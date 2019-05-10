#include "shader.hpp"

namespace AgpuVulkan
{

AVkShader::AVkShader(const agpu::device_ref &device)
    : device(device)
{
    sourceSize = 0;
    shaderModule = VK_NULL_HANDLE;
}

AVkShader::~AVkShader()
{
    if (shaderModule)
        vkDestroyShaderModule(deviceForVk->device, shaderModule, nullptr);
}

agpu::shader_ref AVkShader::create(const agpu::device_ref &device, agpu_shader_type type)
{
    auto result = agpu::makeObject<AVkShader> (device);
    auto shader = result.as<AVkShader> ();
    shader->type = type;
    return result;
}

agpu_error AVkShader::setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
    CHECK_POINTER(sourceText);
    if (language != AGPU_SHADER_LANGUAGE_SPIR_V || sourceTextLength < 0)
        return AGPU_UNSUPPORTED;

    if (sourceTextLength % 4 != 0)
        return AGPU_INVALID_PARAMETER;

    source.reset(new uint8_t[sourceTextLength]);
    memcpy(source.get(), sourceText, sourceTextLength);
    sourceSize = sourceTextLength;
    return AGPU_OK;
}

agpu_error AVkShader::compileShader(agpu_cstring options)
{
    VkShaderModuleCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkShaderModuleCreateInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = reinterpret_cast<uint32_t*> (source.get());
    createInfo.codeSize = sourceSize;

    auto error = vkCreateShaderModule(deviceForVk->device, &createInfo, nullptr, &shaderModule);
    CONVERT_VULKAN_ERROR(error);

    return AGPU_OK;
}

agpu_size AVkShader::getCompilationLogLength()
{
    return 0;
}

agpu_error AVkShader::getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
