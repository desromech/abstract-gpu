#include "shader.hpp"

_agpu_shader::_agpu_shader()
{

}

void _agpu_shader::lostReferences()
{

}

_agpu_shader *_agpu_shader::create(agpu_device *device, agpu_shader_type type)
{
    auto shader = new agpu_shader;
    shader->device = device;
    shader->type = type;
    shader->shaderLanguage = AGPU_SHADER_LANGUAGE_BINARY;
    return shader;
}

agpu_error _agpu_shader::setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
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

agpu_error _agpu_shader::compileShader(agpu_cstring options)
{
    if (shaderLanguage == AGPU_SHADER_LANGUAGE_BINARY)
        return performReflection();

    if (shaderLanguage == AGPU_SHADER_LANGUAGE_HLSL)
        return compileHlslShader(options);

    compilationLog = "Unsupported shader language.";
    return AGPU_UNSUPPORTED;
}

agpu_error _agpu_shader::compileHlslShader(agpu_cstring options)
{
    UINT compileFlags = 0;
    if (device->isDebugEnabled)
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

    // Perform reflection
    return performReflection();
}

const char *_agpu_shader::getHlslTarget()
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

agpu_error _agpu_shader::performReflection()
{
    // Get the reflection interface.
    ComPtr<ID3D12ShaderReflection> reflection;
    ERROR_IF_FAILED(D3DReflect(&objectCode[0], objectCode.size(), IID_PPV_ARGS(&reflection)));

    // Get the shader description
    ERROR_IF_FAILED(reflection->GetDesc(&description));

    // Read the constant buffers
    for (UINT i = 0; i < description.ConstantBuffers; ++i)
    {
        
        // Get the buffer data.
        D3D12_SHADER_BUFFER_DESC rawDesc;
        auto bufferReflection = reflection->GetConstantBufferByIndex(i);
        bufferReflection->GetDesc(&rawDesc);

        ShaderConstantBufferDesc bufferDesc;
        bufferDesc.type = rawDesc.Type;
        bufferDesc.flags = rawDesc.uFlags;
        bufferDesc.size = rawDesc.Size;
        bufferDesc.variables.reserve(rawDesc.Variables);
        bufferDesc.bindingPoint = -1;

        for (UINT j = 0; j < rawDesc.Variables; ++j)
        {
            D3D12_SHADER_VARIABLE_DESC rawVarDesc;
            auto var = bufferReflection->GetVariableByIndex(j);
            var->GetDesc(&rawVarDesc);

            ShaderVariableDesc variable;
            variable.name = rawVarDesc.Name;
            bufferDesc.variables.push_back(variable);
        }

        constantBuffers.insert(std::make_pair(rawDesc.Name, bufferDesc));
    }

    // Read the bound resources.
    constantBufferBindings = 0;
    for (UINT i = 0; i < description.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC rawDesc;
        ERROR_IF_FAILED(reflection->GetResourceBindingDesc(i, &rawDesc));

        if (rawDesc.Type == D3D_CT_CBUFFER)
        {
            constantBuffers[rawDesc.Name].bindingPoint = int(rawDesc.BindPoint);
            for (UINT i = 0; i < rawDesc.BindCount; ++i)
            {
                auto point = rawDesc.BindPoint + i;
                constantBufferBindings |= 1 << point;
            }
        }
    }

    return AGPU_OK;
}

agpu_size _agpu_shader::getShaderCompilationLogLength()
{
    return compilationLog.size();
}

agpu_error _agpu_shader::getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(buffer);
    strncpy(buffer, compilationLog.c_str(), buffer_size);
    return AGPU_OK;
}

agpu_error _agpu_shader::bindAttributeLocation(agpu_cstring name, agpu_int location)
{
    // Nothing to do.
    return AGPU_OK;
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
    CHECK_POINTER(shader);
    return shader->setShaderSource(language,sourceText, sourceTextLength);
}

AGPU_EXPORT agpu_error agpuCompileShader(agpu_shader* shader, agpu_cstring options)
{
    CHECK_POINTER(shader);
    return shader->compileShader(options);
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength(agpu_shader* shader)
{
    CHECK_POINTER(shader);
    return shader->getShaderCompilationLogLength();
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog(agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(shader);
    return shader->getShaderCompilationLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuBindAttributeLocation(agpu_shader* shader, agpu_cstring name, agpu_int location)
{
    return shader->bindAttributeLocation(name, location);
}
