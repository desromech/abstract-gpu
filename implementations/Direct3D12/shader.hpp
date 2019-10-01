#ifndef AGPU_D3D12_SHADER_HPP
#define AGPU_D3D12_SHADER_HPP

#include <map>
#include <vector>
#include "device.hpp"

namespace AgpuD3D12
{

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

class ADXShader: public agpu::shader
{
public:
    ADXShader(const agpu::device_ref &cdevice);
    ~ADXShader();

    static agpu::shader_ref create(const agpu::device_ref &device, agpu_shader_type type);

    virtual agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength) override;
    virtual agpu_error compileShader(agpu_cstring options) override;
    virtual agpu_size getCompilationLogLength() override;
    virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) override;

public:
    agpu_error getShaderBytecodeForEntryPoint(const agpu::shader_signature_ref &shaderSignature, agpu_shader_type type, agpu_cstring entry_point, D3D12_SHADER_BYTECODE *out);

    agpu_shader_type type;
    agpu_shader_language shaderLanguage;
    agpu::device_ref device;

    std::vector<char> sourceCode;
    std::vector<char> objectCode;
    std::string compilationLog;

private:
    agpu_error compileHlslShader(agpu_cstring options);
    const char *getHlslTarget();
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_SHADER_HPP
