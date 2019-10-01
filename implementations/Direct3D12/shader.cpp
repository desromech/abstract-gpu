#include "shader.hpp"
#include <d3dcompiler.h>

namespace AgpuD3D12
{

ADXShader::ADXShader(const agpu::device_ref &cdevice)
    : device(cdevice)
{

}

ADXShader::~ADXShader()
{

}

agpu::shader_ref ADXShader::create(const agpu::device_ref &device, agpu_shader_type type)
{
    auto shader = agpu::makeObject<ADXShader> (device);
    auto adxShader = shader.as<ADXShader> ();
    adxShader->type = type;
    adxShader->shaderLanguage = AGPU_SHADER_LANGUAGE_BINARY;
    return shader;
}

agpu_error ADXShader::setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
    CHECK_POINTER(sourceText);

    // Check the language
    if (language != AGPU_SHADER_LANGUAGE_HLSL && language != AGPU_SHADER_LANGUAGE_BINARY)
        return AGPU_UNSUPPORTED;
    shaderLanguage = language;

    // If the shader is binary
    if (language == AGPU_SHADER_LANGUAGE_BINARY)
    {
        if (sourceTextLength < 0)
            return AGPU_ERROR;

        // If the program is a binary object, just copy.
        objectCode.resize(sourceTextLength);
        memcpy(&objectCode[0], sourceText, sourceTextLength);
        return AGPU_OK;
    }

    // Ensure the source text length.
    if (sourceTextLength < 0)
        sourceTextLength = (agpu_string_length)strlen(sourceText);

    // Copy the source code.
    sourceCode.resize(sourceTextLength);
    strncpy(&sourceCode[0], sourceText, sourceTextLength);

    return AGPU_OK;
}

agpu_error ADXShader::compileShader(agpu_cstring options)
{
    if (shaderLanguage == AGPU_SHADER_LANGUAGE_BINARY)
        return AGPU_OK;

    if (shaderLanguage == AGPU_SHADER_LANGUAGE_HLSL)
        return compileHlslShader(options);

    compilationLog = "Unsupported shader language.";
    return AGPU_UNSUPPORTED;
}

agpu_error ADXShader::compileHlslShader(agpu_cstring options)
{
    UINT compileFlags = 0;
    if (deviceForDX->isDebugEnabled)
        compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;
    auto res = D3DCompile(&sourceCode[0], sourceCode.size(), "agpuShader", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", getHlslTarget(), compileFlags, 0, &shaderBlob, &errorBlob);
    if (FAILED(res))
    {
        if (errorBlob)
        {
            const char *buffer = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());
            compilationLog = std::string(buffer, buffer + errorBlob->GetBufferSize());
            return AGPU_COMPILATION_ERROR;
        }
    }

    // Success
    const char *rawObject = reinterpret_cast<const char*> (shaderBlob->GetBufferPointer());
    objectCode = std::vector<char> (rawObject, rawObject + shaderBlob->GetBufferSize());

    return AGPU_OK;
}

const char *ADXShader::getHlslTarget()
{
    switch (type)
    {
    case AGPU_VERTEX_SHADER:
        return "vs_5_0";
    case AGPU_FRAGMENT_SHADER:
        return "ps_5_0";
    case AGPU_GEOMETRY_SHADER:
        return "gs_5_0";
    case AGPU_COMPUTE_SHADER:
        return "cs_5_0";
    case AGPU_TESSELLATION_CONTROL_SHADER:
        return "hs_5_0";
    case AGPU_TESSELLATION_EVALUATION_SHADER:
        return "ds_5_0";
    default:
        abort();
    }
}

agpu_size ADXShader::getCompilationLogLength()
{
    return compilationLog.size();
}

agpu_error ADXShader::getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(buffer);
    strncpy(buffer, compilationLog.c_str(), buffer_size);
    return AGPU_OK;
}

agpu_error ADXShader::getShaderBytecodeForEntryPoint(const agpu::shader_signature_ref &shaderSignature, agpu_shader_type type, agpu_cstring entry_point, D3D12_SHADER_BYTECODE *out)
{
    out->pShaderBytecode = &objectCode[0];
    out->BytecodeLength = objectCode.size();
    return AGPU_OK;
}

} // End of namespace AgpuD3D12
