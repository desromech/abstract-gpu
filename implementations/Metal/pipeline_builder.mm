#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"

_agpu_pipeline_builder::_agpu_pipeline_builder(agpu_device *device)
    : device(device)
{
    descriptor = nil;
}

void _agpu_pipeline_builder::lostReferences()
{
    if(descriptor)
        [descriptor release];
}

_agpu_pipeline_builder *_agpu_pipeline_builder::create(agpu_device *device)
{
    auto descriptor = [MTLRenderPipelineDescriptor new];
    descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

    auto result = new agpu_pipeline_builder(device);
    result->descriptor = descriptor;
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
        return nullptr;
    }

    return agpu_pipeline_state::create(device, pipelineState);
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
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setBlendFunction ( agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setColorMask ( agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setStencilFrontFace ( agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setStencilBackFace ( agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount ( agpu_int count )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat ( agpu_uint index, agpu_texture_format format )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat ( agpu_texture_format format )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType ( agpu_primitive_topology type )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setVertexLayout ( agpu_vertex_layout* layout )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setPipelineShaderSignature ( agpu_shader_signature* signature )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setSampleDescription ( agpu_uint sample_count, agpu_uint sample_quality )
{
    return AGPU_UNIMPLEMENTED;
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
