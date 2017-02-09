#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "vertex_layout.hpp"
#include "shader_signature.hpp"
#include "texture_format.hpp"

inline MTLBlendFactor mapBlendFactor(agpu_blending_factor factor, bool alpha)
{
    switch(factor)
    {
    case AGPU_BLENDING_ZERO: return MTLBlendFactorZero;
    case AGPU_BLENDING_ONE: return MTLBlendFactorOne;
    case AGPU_BLENDING_SRC_COLOR: return MTLBlendFactorSourceColor;
    case AGPU_BLENDING_INVERTED_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
    case AGPU_BLENDING_SRC_ALPHA: return MTLBlendFactorSourceAlpha;
    case AGPU_BLENDING_INVERTED_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
    case AGPU_BLENDING_DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
    case AGPU_BLENDING_INVERTED_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
    case AGPU_BLENDING_DEST_COLOR: return MTLBlendFactorDestinationColor;
    case AGPU_BLENDING_INVERTED_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
    case AGPU_BLENDING_SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
    case AGPU_BLENDING_CONSTANT_FACTOR: return alpha ? MTLBlendFactorBlendColor : MTLBlendFactorBlendAlpha;
    case AGPU_BLENDING_INVERTED_CONSTANT_FACTOR: return alpha ? MTLBlendFactorOneMinusBlendColor : MTLBlendFactorOneMinusBlendAlpha;
/*    case AGPU_BLENDING_SRC_1COLOR: return MTLBlendFactorSource1Color;
    case AGPU_BLENDING_INVERTED_SRC_1COLOR: return MTLBlendFactorOneMinusSource1Color;
    case AGPU_BLENDING_SRC_1ALPHA: return MTLBlendFactorSource1Alpha;
    case AGPU_BLENDING_INVERTED_SRC_1ALPHA: return MTLBlendFactorOneMinusSource1Alpha;
    */
    default: abort();
    }
}

inline MTLBlendOperation mapBlendOperation(agpu_blending_operation operation)
{
    switch(operation)
    {
    case AGPU_BLENDING_OPERATION_ADD: return MTLBlendOperationAdd;
    case AGPU_BLENDING_OPERATION_SUBTRACT: return MTLBlendOperationSubtract;
    case AGPU_BLENDING_OPERATION_REVERSE_SUBTRACT: return MTLBlendOperationReverseSubtract;
    case AGPU_BLENDING_OPERATION_MIN: return MTLBlendOperationMin;
    case AGPU_BLENDING_OPERATION_MAX: return MTLBlendOperationMax;
    default: abort();
    }
}

inline MTLCompareFunction mapCompareFunction(agpu_compare_function function)
{
    switch(function)
    {
    case AGPU_ALWAYS: return MTLCompareFunctionAlways;
    case AGPU_NEVER: return MTLCompareFunctionNever;
    case AGPU_LESS: return MTLCompareFunctionLess;
    case AGPU_LESS_EQUAL: return MTLCompareFunctionLessEqual;
    case AGPU_EQUAL: return MTLCompareFunctionEqual;
    case AGPU_NOT_EQUAL: return MTLCompareFunctionNotEqual;
    case AGPU_GREATER: return MTLCompareFunctionGreater;
    case AGPU_GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
    default: abort();
    }
}

inline MTLStencilOperation mapStencilOperation(agpu_stencil_operation operation)
{
    switch(operation)
    {
    case AGPU_KEEP: return MTLStencilOperationKeep;
	case AGPU_ZERO: return MTLStencilOperationZero;
	case AGPU_REPLACE: return MTLStencilOperationReplace;
	case AGPU_INVERT: return MTLStencilOperationInvert;
	case AGPU_INCREASE: return MTLStencilOperationIncrementClamp;
	case AGPU_INCREASE_WRAP: return MTLStencilOperationIncrementWrap;
	case AGPU_DECREASE: return MTLStencilOperationDecrementClamp;
	case AGPU_DECREASE_WRAP: return MTLStencilOperationDecrementWrap;
    default: abort();
    }
}

_agpu_pipeline_builder::_agpu_pipeline_builder(agpu_device *device)
    : device(device)
{
    descriptor = nil;
    depthStencilDescriptor = nil;
    backStencilDescriptor = nil;
    frontStencilDescriptor = nil;

    shaderSignature = nullptr;
    renderTargetCount = 1;
    depthEnabled = false;
    stencilEnabled = false;
}

void _agpu_pipeline_builder::lostReferences()
{
    if(descriptor)
        [descriptor release];
    if(depthStencilDescriptor)
        [depthStencilDescriptor release];
}

_agpu_pipeline_builder *_agpu_pipeline_builder::create(agpu_device *device)
{
    auto descriptor = [MTLRenderPipelineDescriptor new];
    descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

    auto result = new agpu_pipeline_builder(device);
    result->descriptor = descriptor;
    result->depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    result->backStencilDescriptor = [MTLStencilDescriptor new];
    result->frontStencilDescriptor = [MTLStencilDescriptor new];

    return result;
}

agpu_pipeline_state* _agpu_pipeline_builder::build ( )
{
    NSError *error;
    auto pipelineState = [device->device newRenderPipelineStateWithDescriptor: descriptor error: &error];
    if(!pipelineState)
    {
        auto description = [error localizedDescription];
        buildingLog = [description UTF8String];
        printf("Failed to build pipeline state: %s\n", buildingLog.c_str());
        return nullptr;
    }

    depthStencilDescriptor.backFaceStencil = stencilEnabled ? backStencilDescriptor : nil;
    depthStencilDescriptor.frontFaceStencil = stencilEnabled ? frontStencilDescriptor : nil;

    return agpu_pipeline_state::create(device, this, pipelineState);
}

agpu_error _agpu_pipeline_builder::attachShader ( agpu_shader* shader )
{
    CHECK_POINTER(shader);
    CHECK_POINTER(shader->function);

    switch(shader->type)
    {
    case AGPU_VERTEX_SHADER:
        descriptor.vertexFunction = shader->function;
        break;
    case AGPU_FRAGMENT_SHADER:
        descriptor.fragmentFunction = shader->function;
        break;
    default:
        return AGPU_UNSUPPORTED;
    }

    return AGPU_OK;
}

agpu_size _agpu_pipeline_builder::getBuildingLogLength (  )
{
    return buildingLog.size();
}

agpu_error _agpu_pipeline_builder::getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(buffer);
    memcpy(buffer, buildingLog.data(), std::min((size_t)buffer_size, buildingLog.size()));
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendState ( agpu_int renderTargetMask, agpu_bool enabled )
{
    for(agpu_uint i = 0; i < renderTargetCount; ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        descriptor.colorAttachments[i].blendingEnabled = enabled;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendFunction ( agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
    auto sourceRGBBlendFactor = mapBlendFactor(sourceFactor, false);
    auto destinationRGBBlendFactor = mapBlendFactor(destFactor, false);
    auto rgbBlendOperation = mapBlendOperation(colorOperation);

    auto sourceAlphaBlendFactor = mapBlendFactor(sourceAlphaFactor, true);
    auto destinationAlphaBlendFactor = mapBlendFactor(destAlphaFactor, true);
    auto alphaBlendOperation = mapBlendOperation(alphaOperation);

    for(agpu_uint i = 0; i < renderTargetCount; ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        auto attachment = descriptor.colorAttachments[i];
        attachment.sourceRGBBlendFactor = sourceRGBBlendFactor;
        attachment.destinationRGBBlendFactor = destinationRGBBlendFactor;
        attachment.rgbBlendOperation = rgbBlendOperation;

        attachment.sourceAlphaBlendFactor = sourceAlphaBlendFactor;
        attachment.destinationAlphaBlendFactor = destinationAlphaBlendFactor;
        attachment.alphaBlendOperation = alphaBlendOperation;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setColorMask ( agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
    auto mask = 0;
    if(redEnabled)
        mask |= MTLColorWriteMaskRed;
    if(greenEnabled)
        mask |= MTLColorWriteMaskGreen;
    if(blueEnabled)
        mask |= MTLColorWriteMaskBlue;
    if(alphaEnabled)
        mask |= MTLColorWriteMaskAlpha;

    for(agpu_uint i = 0; i < renderTargetCount; ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        descriptor.colorAttachments[i].writeMask = mask;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setFrontFace ( agpu_face_winding winding )
{
    switch(winding)
    {
    case AGPU_CLOCKWISE:
        commandState.frontFace = MTLWindingClockwise;
        return AGPU_OK;
    case AGPU_COUNTER_CLOCKWISE:
        commandState.frontFace = MTLWindingCounterClockwise;
        return AGPU_OK;
    default:
        return AGPU_INVALID_PARAMETER;
    }
}

agpu_error _agpu_pipeline_builder::setCullMode ( agpu_cull_mode mode )
{
    switch(mode)
    {
    case AGPU_CULL_MODE_NONE:
        commandState.cullMode = MTLCullModeNone;
        descriptor.rasterizationEnabled = YES;
        return AGPU_OK;
    case AGPU_CULL_MODE_FRONT:
        commandState.cullMode = MTLCullModeFront;
        descriptor.rasterizationEnabled = YES;
        return AGPU_OK;
    case AGPU_CULL_MODE_BACK:
        commandState.cullMode = MTLCullModeBack;
        descriptor.rasterizationEnabled = YES;
        return AGPU_OK;
    case AGPU_CULL_MODE_FRONT_AND_BACK:
        commandState.cullMode = MTLCullModeFront;
        descriptor.rasterizationEnabled = NO;
        return AGPU_OK;
    default:
        return AGPU_INVALID_PARAMETER;
    }
}

agpu_error _agpu_pipeline_builder::setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
    depthEnabled = enabled;
    depthStencilDescriptor.depthCompareFunction = mapCompareFunction(function);
    depthStencilDescriptor.depthWriteEnabled = writeMask;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
    stencilEnabled = enabled;
    backStencilDescriptor.writeMask = writeMask;
    backStencilDescriptor.readMask = readMask;
    frontStencilDescriptor.writeMask = writeMask;
    frontStencilDescriptor.readMask = readMask;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilFrontFace ( agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    frontStencilDescriptor.stencilFailureOperation = mapStencilOperation(stencilFailOperation);
    frontStencilDescriptor.depthFailureOperation = mapStencilOperation(depthFailOperation);
    frontStencilDescriptor.depthStencilPassOperation = mapStencilOperation(stencilDepthPassOperation);
    frontStencilDescriptor.stencilCompareFunction = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilBackFace ( agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    backStencilDescriptor.stencilFailureOperation = mapStencilOperation(stencilFailOperation);
    backStencilDescriptor.depthFailureOperation = mapStencilOperation(depthFailOperation);
    backStencilDescriptor.depthStencilPassOperation = mapStencilOperation(stencilDepthPassOperation);
    backStencilDescriptor.stencilCompareFunction = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount ( agpu_int count )
{
    renderTargetCount = count;
    for(agpu_uint i = 0; i < count; ++i)
        descriptor.colorAttachments[i].pixelFormat = MTLPixelFormatBGRA8Unorm;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat ( agpu_uint index, agpu_texture_format format )
{
    if(index >= renderTargetCount)
        return AGPU_OUT_OF_BOUNDS;

    descriptor.colorAttachments[index].pixelFormat = mapTextureFormat(format);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat ( agpu_texture_format format )
{
    if(hasDepthComponent(format))
        descriptor.depthAttachmentPixelFormat = mapTextureFormat(format);
    else
        descriptor.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
    if(hasStencilComponent(format))
        descriptor.stencilAttachmentPixelFormat = mapTextureFormat(format);
    else
        descriptor.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType ( agpu_primitive_topology type )
{
    switch(type)
    {
    case AGPU_POINTS:
        commandState.primitiveType = MTLPrimitiveTypePoint;
        break;
    case AGPU_LINES:
        commandState.primitiveType = MTLPrimitiveTypeLine;
        break;
    case AGPU_LINE_STRIP:
        commandState.primitiveType = MTLPrimitiveTypeLineStrip;
        break;
    case AGPU_TRIANGLES:
        commandState.primitiveType = MTLPrimitiveTypeTriangle;
        break;
    case AGPU_TRIANGLE_STRIP:
        commandState.primitiveType = MTLPrimitiveTypeTriangleStrip;
        break;
    default: return AGPU_UNSUPPORTED;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setVertexLayout ( agpu_vertex_layout* layout )
{
    CHECK_POINTER(layout);
    descriptor.vertexDescriptor = layout->vertexDescriptor;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPipelineShaderSignature ( agpu_shader_signature* signature )
{
    if(signature)
        signature->retain();
    if(shaderSignature)
        shaderSignature->release();
    shaderSignature = signature;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setSampleDescription ( agpu_uint sample_count, agpu_uint sample_quality )
{
    descriptor.sampleCount = sample_count;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference ( agpu_pipeline_builder* pipeline_builder )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder ( agpu_pipeline_builder* pipeline_builder )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState ( agpu_pipeline_builder* pipeline_builder )
{
    if(!pipeline_builder)
        return nullptr;
    return pipeline_builder->build();
}

AGPU_EXPORT agpu_error agpuAttachShader ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->attachShader(shader);
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength ( agpu_pipeline_builder* pipeline_builder )
{
    if(!pipeline_builder)
        return 0;
    return pipeline_builder->getBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->getBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetBlendState ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setBlendState(renderTargetMask, enabled);
}

AGPU_EXPORT agpu_error agpuSetBlendFunction ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
}

AGPU_EXPORT agpu_error agpuSetColorMask ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
}

AGPU_EXPORT agpu_error agpuSetFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_face_winding winding )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setFrontFace(winding);
}

AGPU_EXPORT agpu_error agpuSetCullMode ( agpu_pipeline_builder* pipeline_builder, agpu_cull_mode mode )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setCullMode(mode);
}

AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setDepthState(enabled, writeMask, function);
}

AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilState(enabled, writeMask, readMask);
}

AGPU_EXPORT agpu_error agpuSetStencilFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetStencilBackFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setRenderTargetCount(count);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetFormat ( agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setRenderTargetFormat(index, format);
}

AGPU_EXPORT agpu_error agpuSetDepthStencilFormat ( agpu_pipeline_builder* pipeline_builder, agpu_texture_format format )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setDepthStencilFormat(format);
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuSetVertexLayout ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setVertexLayout(layout);
}

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature ( agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPipelineShaderSignature(signature);
}

AGPU_EXPORT agpu_error agpuSetSampleDescription ( agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setSampleDescription(sample_count, sample_quality);
}
