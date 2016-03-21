#include "shader.hpp"

_agpu_shader::_agpu_shader(agpu_device *device)
    : device(device)
{
    sourceSize = 0;
    shaderModule = nullptr;
}

void _agpu_shader::lostReferences()
{
    if (shaderModule)
        vkDestroyShaderModule(device->device, shaderModule, nullptr);
}

agpu_shader *_agpu_shader::create(agpu_device *device, agpu_shader_type type)
{
    std::unique_ptr<agpu_shader> result(new agpu_shader(device));
    result->type = type;
    return result.release();
}

agpu_error _agpu_shader::setSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
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

agpu_error _agpu_shader::compile(agpu_cstring options)
{
    VkShaderModuleCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkShaderModuleCreateInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = reinterpret_cast<uint32_t*> (source.get());
    createInfo.codeSize = sourceSize;

    auto error = vkCreateShaderModule(device->device, &createInfo, nullptr, &shaderModule);
    CONVERT_VULKAN_ERROR(error);

    return AGPU_OK;
}

agpu_size _agpu_shader::getCompilationLogLength()
{
    return 0;
}

agpu_error _agpu_shader::getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderReference(agpu_shader* shader)
{
    CHECK_POINTER(shader);
    return shader->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShader(agpu_shader* shader)
{
    CHECK_POINTER(shader);
    return shader->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSource(agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
    CHECK_POINTER(shader);
    return shader->setSource(language, sourceText, sourceTextLength);
}

AGPU_EXPORT agpu_error agpuCompileShader(agpu_shader* shader, agpu_cstring options)
{
    CHECK_POINTER(shader);
    return shader->compile(options);
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength(agpu_shader* shader)
{
    return shader->getCompilationLogLength();
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog(agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(shader);
    return shader->getCompilationLog(buffer_size, buffer);
}
