#ifndef AGPU_VULKAN_SHADER_HPP
#define AGPU_VULKAN_SHADER_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkShader : public agpu::shader
{
public:
    AVkShader(const agpu::device_ref &device);
    ~AVkShader();

    static agpu::shader_ref create(const agpu::device_ref &device, agpu_shader_type type);

    virtual agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength) override;
    virtual agpu_error compileShader(agpu_cstring options) override;
    virtual agpu_size getCompilationLogLength() override;
    virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) override;

    agpu::device_ref device;
    VkShaderModule shaderModule;

    agpu_shader_type type;
    std::unique_ptr<uint8_t[]> source;
    size_t sourceSize;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_SHADER_HPP
