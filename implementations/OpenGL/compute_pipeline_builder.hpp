#ifndef AGPU_OPENGL_COMPUTE_PIPELINE_BUILDER_HPP
#define AGPU_OPENGL_COMPUTE_PIPELINE_BUILDER_HPP

#include "device.hpp"
#include "shader.hpp"
#include <utility>
#include <string>
#include <set>

namespace AgpuGL
{

struct GLComputePipelineBuilder : public agpu::compute_pipeline_builder
{
public:
    GLComputePipelineBuilder(const agpu::device_ref &device);
    ~GLComputePipelineBuilder();

    static agpu::compute_pipeline_builder_ref create(const agpu::device_ref &device);

    virtual agpu::pipeline_state_ptr build() override;
    virtual agpu_error attachShader(const agpu::shader_ref &shader ) override;
    virtual agpu_error attachShaderWithEntryPoint(const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point) override;
    virtual agpu_size getBuildingLogLength() override;
    virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) override;
    virtual agpu_error setShaderSignature (const agpu::shader_signature_ref &newSignature) override;

    agpu::device_ref device;

private:
    agpu::shader_ref shader;
    agpu::shader_signature_ref shaderSignature;
    std::string shaderEntryPointName;
    std::string errorMessages;

private:
	void buildTextureWithSampleCombinationMapInto(TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations);
};

} // End of namespace AgpuGL

#endif //AGPU_OPENGL_COMPUTE_PIPELINE_BUILDER_HPP
