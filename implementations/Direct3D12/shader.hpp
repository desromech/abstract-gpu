#ifndef AGPU_D3D12_SHADER_HPP
#define AGPU_D3D12_SHADER_HPP

#include <map>
#include <vector>
#include "device.hpp"

struct ShaderVariableDesc
{
    std::string name;
};

struct ShaderConstantBufferDesc
{
    D3D_CBUFFER_TYPE type;
    size_t size;
    UINT flags;
    int bindingPoint;
    std::vector<ShaderVariableDesc> variables;
};

struct _agpu_shader: public Object<_agpu_shader>
{
public:
    _agpu_shader();

    void lostReferences();

    static _agpu_shader *create(agpu_device *device, agpu_shader_type type);

    AGPU_EXPORT agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
    AGPU_EXPORT agpu_error compileShader(agpu_cstring options);
    AGPU_EXPORT agpu_size getShaderCompilationLogLength();
    AGPU_EXPORT agpu_error getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer);

    AGPU_EXPORT agpu_error bindAttributeLocation(agpu_cstring name, agpu_int location);

public:
    D3D12_SHADER_BYTECODE getShaderBytecode()
    {
        return {&objectCode[0], objectCode.size()};
    }

    agpu_shader_type type;
    agpu_shader_language shaderLanguage;
    agpu_device *device;

    std::vector<char> sourceCode;
    std::vector<char> objectCode;
    std::string compilationLog;

    D3D12_SHADER_DESC description;
    std::map<std::string, ShaderConstantBufferDesc> constantBuffers;
    int constantBufferBindings;

private:
    agpu_error compileHlslShader(agpu_cstring options);
    const char *getHlslTarget();
    agpu_error performReflection();
};

#endif //AGPU_D3D12_SHADER_HPP
