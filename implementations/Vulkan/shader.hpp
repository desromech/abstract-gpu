#ifndef AGPU_VULKAN_SHADER_HPP
#define AGPU_VULKAN_SHADER_HPP

#include "device.hpp"

struct _agpu_shader : public Object<_agpu_shader>
{
    _agpu_shader(agpu_device *device);
    void lostReferences();

    static agpu_shader *create(agpu_device *device, agpu_shader_type type);

    agpu_error setSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
    agpu_error compile(agpu_cstring options);
    agpu_size getCompilationLogLength();
    agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer);

    agpu_device *device;
    VkShaderModule shaderModule;

    agpu_shader_type type;
    std::unique_ptr<uint8_t[]> source;
    size_t sourceSize;
};

#endif //AGPU_VULKAN_SHADER_HPP
