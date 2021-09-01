#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "vertex_layout.hpp"
#include "shader_signature.hpp"
#include "texture_format.hpp"
#include "constants.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{
    
AMtlGraphicsPipelineBuilder::AMtlGraphicsPipelineBuilder(const agpu::device_ref &device)
    : device(device)
{
    AgpuProfileConstructor(AMtlGraphicsPipelineBuilder);
    renderTargetCount = 1;
    depthEnabled = false;
    stencilEnabled = false;
    
    hasDepthBias = false;
    depthBiasConstantFactor = 0.0f;
    depthBiasClamp = 0.0f;
    depthBiasSlopeFactor = 0.0f;
}

AMtlGraphicsPipelineBuilder::~AMtlGraphicsPipelineBuilder()
{
    AgpuProfileDestructor(AMtlGraphicsPipelineBuilder);
}

agpu::pipeline_builder_ref AMtlGraphicsPipelineBuilder::create(const agpu::device_ref &device)
{
    auto descriptor = [MTLRenderPipelineDescriptor new];
    descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

    auto result = agpu::makeObject<AMtlGraphicsPipelineBuilder> (device);
    auto builder = result.as<AMtlGraphicsPipelineBuilder> ();
    builder->descriptor = descriptor;
    builder->depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    builder->backStencilDescriptor = [MTLStencilDescriptor new];
    builder->frontStencilDescriptor = [MTLStencilDescriptor new];

    return result;
}

agpu::pipeline_state_ptr AMtlGraphicsPipelineBuilder::build()
{
    @autoreleasepool {
        NSError *error;

        bool succeded = true;
        for(auto &attachedShader : attachedShaders)
        {
            const auto &shader = attachedShader.shader;
            if(!shader)
            {
                succeded = false;
                break;
            }

            AMtlShaderForSignatureRef shaderInstance;
            auto error = shader.as<AMtlShader> ()->getOrCreateShaderInstanceForSignature(shaderSignature, attachedShader.entryPoint, attachedShader.stage, &buildingLog, &shaderInstance);
            if(error || !shaderInstance || !shaderInstance->function)
            {
                succeded = false;
                break;
            }

            switch(attachedShader.stage)
            {
            case AGPU_VERTEX_SHADER:
                descriptor.vertexFunction = shaderInstance->function;
                break;
            case AGPU_FRAGMENT_SHADER:
                descriptor.fragmentFunction = shaderInstance->function;
                break;
            default:
                succeded = false;
                break;
            }

            if(!succeded)
                break;
        }

        if(!succeded)
            return nullptr;

        auto amtlShaderSignature = shaderSignature.as<AMtlShaderSignature> ();
        if(vertexLayout)
        {
            auto amtlVertexLayout = vertexLayout.as<AMtlVertexLayout> ();
            auto vertexDescriptor = amtlVertexLayout->createVertexDescriptor(amtlShaderSignature->boundVertexBufferCount);
            descriptor.vertexDescriptor = vertexDescriptor;
        }

        auto pipelineState = [deviceForMetal->device newRenderPipelineStateWithDescriptor: descriptor error: &error];
        if(!pipelineState)
        {
            auto description = [error localizedDescription];
            buildingLog = [description UTF8String];
            printf("Failed to build pipeline state: %s\n", buildingLog.c_str());
            return nullptr;
        }

        depthStencilDescriptor.backFaceStencil = stencilEnabled ? backStencilDescriptor : nil;
        depthStencilDescriptor.frontFaceStencil = stencilEnabled ? frontStencilDescriptor : nil;

        return AMtlPipelineState::createRender(device, this, pipelineState).disown();
    }
}

agpu_error AMtlGraphicsPipelineBuilder::attachShader(const agpu::shader_ref &shader)
{
    CHECK_POINTER(shader);
    return attachShaderWithEntryPoint(shader, shader.as<AMtlShader> ()->type, "main");
}

agpu_error AMtlGraphicsPipelineBuilder::attachShaderWithEntryPoint(const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point)
{
    CHECK_POINTER(shader);
    CHECK_POINTER(entry_point);
    if(type == AGPU_LIBRARY_SHADER)
        return AGPU_INVALID_PARAMETER;

    AgpuShaderAttachment attachment;
    attachment.shader = shader;
    attachment.entryPoint = entry_point;
    attachment.stage = type;
    attachedShaders.push_back(attachment);
    return AGPU_OK;
}

agpu_size AMtlGraphicsPipelineBuilder::getBuildingLogLength()
{
    return buildingLog.size();
}

agpu_error AMtlGraphicsPipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(buffer);
    memcpy(buffer, buildingLog.data(), std::min((size_t)buffer_size, buildingLog.size()));
    if(buffer_size > buildingLog.size())
        buffer[buildingLog.size()] = 0;
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    for(agpu_uint i = 0; i < renderTargetCount; ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        descriptor.colorAttachments[i].blendingEnabled = enabled;
    }

    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
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

agpu_error AMtlGraphicsPipelineBuilder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
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

agpu_error AMtlGraphicsPipelineBuilder::setFrontFace(agpu_face_winding winding)
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

agpu_error AMtlGraphicsPipelineBuilder::setCullMode(agpu_cull_mode mode)
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

agpu_error AMtlGraphicsPipelineBuilder::setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
    hasDepthBias = true;
    depthBiasConstantFactor = constant_factor;
    depthBiasClamp = clamp;
    depthBiasSlopeFactor = slope_factor;
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    depthEnabled = enabled;
    if(depthEnabled)
    {
        depthStencilDescriptor.depthCompareFunction = mapCompareFunction(function);
        depthStencilDescriptor.depthWriteEnabled = writeMask;        
    }
    else
    {
        depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionAlways;
        depthStencilDescriptor.depthWriteEnabled = NO;
    }
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    stencilEnabled = enabled;
    backStencilDescriptor.writeMask = writeMask;
    backStencilDescriptor.readMask = readMask;
    frontStencilDescriptor.writeMask = writeMask;
    frontStencilDescriptor.readMask = readMask;
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    frontStencilDescriptor.stencilFailureOperation = mapStencilOperation(stencilFailOperation);
    frontStencilDescriptor.depthFailureOperation = mapStencilOperation(depthFailOperation);
    frontStencilDescriptor.depthStencilPassOperation = mapStencilOperation(stencilDepthPassOperation);
    frontStencilDescriptor.stencilCompareFunction = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    backStencilDescriptor.stencilFailureOperation = mapStencilOperation(stencilFailOperation);
    backStencilDescriptor.depthFailureOperation = mapStencilOperation(depthFailOperation);
    backStencilDescriptor.depthStencilPassOperation = mapStencilOperation(stencilDepthPassOperation);
    backStencilDescriptor.stencilCompareFunction = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setRenderTargetCount(agpu_int count)
{
    for(agpu_uint i = 0; i < renderTargetCount; ++i)
        descriptor.colorAttachments[i].pixelFormat = MTLPixelFormatInvalid;
    renderTargetCount = count;
    for(agpu_uint i = 0; i < count; ++i)
        descriptor.colorAttachments[i].pixelFormat = MTLPixelFormatBGRA8Unorm;
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    if(index >= renderTargetCount)
        return AGPU_OUT_OF_BOUNDS;

    descriptor.colorAttachments[index].pixelFormat = mapTextureFormat(format);
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setDepthStencilFormat(agpu_texture_format format)
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

agpu_error AMtlGraphicsPipelineBuilder::setPrimitiveType(agpu_primitive_topology type)
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

agpu_error AMtlGraphicsPipelineBuilder::setPolygonMode(agpu_polygon_mode mode)
{
    // TODO: Implement myself
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setVertexLayout(const agpu::vertex_layout_ref &layout)
{
    CHECK_POINTER(layout);
    this->vertexLayout = layout;
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    this->shaderSignature = signature;
    return AGPU_OK;
}

agpu_error AMtlGraphicsPipelineBuilder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    descriptor.sampleCount = sample_count;
    return AGPU_OK;
}

} // End of namespace AgpuMetal
