#include "compute_pipeline_builder.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "pipeline_state.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{
    
AMtlComputePipelineBuilder::AMtlComputePipelineBuilder(const agpu::device_ref &device)
    : device(device)
{
    AgpuProfileConstructor(AMtlComputePipelineBuilder);
}

AMtlComputePipelineBuilder::~AMtlComputePipelineBuilder()
{
    AgpuProfileDestructor(AMtlComputePipelineBuilder);
}

agpu::compute_pipeline_builder_ref AMtlComputePipelineBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AMtlComputePipelineBuilder> (device);
}

agpu::pipeline_state_ptr AMtlComputePipelineBuilder::build ()
{
    bool succeded = true;
    if(!shader)
    {
        buildingLog = "Missing compute shader.";
        succeded = false;
        return nullptr;
    }

    @autoreleasepool {
        AMtlShaderForSignatureRef shaderInstance;
        auto error = shader.as<AMtlShader> ()->getOrCreateShaderInstanceForSignature(shaderSignature, shaderEntryPoint, AGPU_COMPUTE_SHADER, &buildingLog, &shaderInstance);
        if(error || !shaderInstance || !shaderInstance->function)
            return nullptr;

        localSize = shaderInstance->localSize;

        NSError *nsError;
        auto pipelineState = [deviceForMetal->device newComputePipelineStateWithFunction: shaderInstance->function error: &nsError];
        if(!pipelineState)
        {
            auto description = [nsError localizedDescription];
            buildingLog = [description UTF8String];
            printf("Failed to build pipeline state: %s\n", buildingLog.c_str());
            return nullptr;
        }

        return AMtlPipelineState::createCompute(device, this, pipelineState).disown();
    }
}

agpu_error AMtlComputePipelineBuilder::attachShader(const agpu::shader_ref &newShader)
{
    return attachShaderWithEntryPoint(newShader, AGPU_COMPUTE_SHADER, "main");
}

agpu_error AMtlComputePipelineBuilder::attachShaderWithEntryPoint(const agpu::shader_ref &newShader, agpu_shader_type type, agpu_cstring entry_point)
{
    CHECK_POINTER(newShader);
    CHECK_POINTER(entry_point);
    if(type != AGPU_COMPUTE_SHADER)
        return AGPU_INVALID_PARAMETER;

    shader = newShader;
    shaderEntryPoint = entry_point;
    return AGPU_OK;
}

agpu_size AMtlComputePipelineBuilder::getBuildingLogLength()
{
    return buildingLog.size();
}

agpu_error AMtlComputePipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(buffer);
    memcpy(buffer, buildingLog.data(), std::min((size_t)buffer_size, buildingLog.size()));
    if(buffer_size > buildingLog.size())
        buffer[buildingLog.size()] = 0;
    return AGPU_OK;
}

agpu_error AMtlComputePipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &newSignature)
{
    shaderSignature = newSignature;
    return AGPU_OK;
}

} // End of namespace AgpuMetal
