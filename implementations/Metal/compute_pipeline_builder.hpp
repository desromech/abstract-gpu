#ifndef AGPU_METAL_COMPUTE_PIPELINE_BUILDER_HPP
#define AGPU_METAL_COMPUTE_PIPELINE_BUILDER_HPP

#include "device.hpp"
#include <string>

struct _agpu_compute_pipeline_builder : public Object<_agpu_compute_pipeline_builder>
{
    _agpu_compute_pipeline_builder(agpu_device *device);
    void lostReferences();

    static _agpu_compute_pipeline_builder *create(agpu_device *device);

    agpu_pipeline_state* build ();
    agpu_error attachShader ( agpu_shader* shader );
    agpu_error attachShaderWithEntryPoint ( agpu_shader* shader, agpu_cstring entry_point );
    agpu_size getPipelineBuildingLogLength ( );
    agpu_error getPipelineBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer );
    agpu_error setShaderSignature ( agpu_shader_signature* newSignature );

    agpu_device *device;

private:
    agpu_shader* shader;
    agpu_shader_signature* shaderSignature;
    std::string shaderEntryPointName;
};

#endif //AGPU_METAL_COMPUTE_PIPELINE_BUILDER_HPP
