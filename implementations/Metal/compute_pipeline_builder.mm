#include "compute_pipeline_builder.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "pipeline_state.hpp"

_agpu_compute_pipeline_builder::_agpu_compute_pipeline_builder(agpu_device *device)
    : device(device)
{
    shader = nullptr;
    shaderSignature = nullptr;
}

void _agpu_compute_pipeline_builder::lostReferences()
{
    if(shader)
        shader->release();
    if(shaderSignature)
        shaderSignature->release();
}

_agpu_compute_pipeline_builder *_agpu_compute_pipeline_builder::create(agpu_device *device)
{
    return new _agpu_compute_pipeline_builder(device);
}

agpu_pipeline_state* _agpu_compute_pipeline_builder::build ()
{
    bool succeded = true;
    if(!shader)
    {
        buildingLog = "Missing compute shader.";
        succeded = false;
        return nullptr;
    }

    agpu_shader_forSignature *shaderInstance = nullptr;
    auto error = shader->getOrCreateShaderInstanceForSignature(shaderSignature, 0, shaderEntryPoint, AGPU_COMPUTE_SHADER, &buildingLog, &shaderInstance);
    if(error || !shaderInstance || !shaderInstance->function)
    {
        if(shaderInstance)
            shaderInstance->release();
        return nullptr;
    }

    localSize = shaderInstance->localSize;

    NSError *nsError;
    auto pipelineState = [device->device newComputePipelineStateWithFunction: shaderInstance->function error: &nsError];
    shaderInstance->release();
    if(!pipelineState)
    {
        auto description = [nsError localizedDescription];
        buildingLog = [description UTF8String];
        printf("Failed to build pipeline state: %s\n", buildingLog.c_str());
        return nullptr;
    }

    return agpu_pipeline_state::createCompute(device, this, pipelineState);
}

agpu_error _agpu_compute_pipeline_builder::attachShader ( agpu_shader* shader )
{
    return attachShaderWithEntryPoint(shader, AGPU_COMPUTE_SHADER, "main");
}

agpu_error _agpu_compute_pipeline_builder::attachShaderWithEntryPoint ( agpu_shader* newShader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(newShader);
    CHECK_POINTER(entry_point);
    if(type != AGPU_COMPUTE_SHADER)
        return AGPU_INVALID_PARAMETER;

    newShader->retain();
    if(shader)
        shader->release();
    shader = newShader;
    shaderEntryPoint = entry_point;
    return AGPU_OK;
}

agpu_size _agpu_compute_pipeline_builder::getPipelineBuildingLogLength ( )
{
    return buildingLog.size();
}

agpu_error _agpu_compute_pipeline_builder::getPipelineBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(buffer);
    memcpy(buffer, buildingLog.data(), std::min((size_t)buffer_size, buildingLog.size()));
    if(buffer_size > buildingLog.size())
        buffer[buildingLog.size()] = 0;
    return AGPU_OK;
}

agpu_error _agpu_compute_pipeline_builder::setShaderSignature ( agpu_shader_signature* newSignature )
{
    if(newSignature)
        newSignature->retain();
    if(shaderSignature)
        shaderSignature->release();
    shaderSignature = newSignature;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddComputePipelineBuilderReference ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseComputePipelineBuilder ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildComputePipelineState ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    if(!compute_pipeline_builder)
        return nullptr;

    return compute_pipeline_builder->build();
}

AGPU_EXPORT agpu_error agpuAttachComputeShader ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShader(shader);
}

AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShaderWithEntryPoint(shader, type, entry_point);
}

AGPU_EXPORT agpu_size agpuGetComputePipelineBuildingLogLength ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    if(!compute_pipeline_builder)
        return 0;
    return compute_pipeline_builder->getPipelineBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetComputePipelineBuildingLog ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->getPipelineBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetComputePipelineShaderSignature ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader_signature* signature )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->setShaderSignature(signature);
}
