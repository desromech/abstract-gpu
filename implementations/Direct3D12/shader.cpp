#include "shader.hpp"

_agpu_shader::_agpu_shader()
{

}

void _agpu_shader::lostReferences()
{

}

// Exported C interface
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
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuCompileShader(agpu_shader* shader, agpu_cstring options)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength(agpu_shader* shader)
{
    return 0;
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog(agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindAttributeLocation(agpu_shader* shader, agpu_cstring name, agpu_int location)
{
    return AGPU_UNIMPLEMENTED;
}
