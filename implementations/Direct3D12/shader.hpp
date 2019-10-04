#ifndef AGPU_D3D12_SHADER_HPP
#define AGPU_D3D12_SHADER_HPP

#include <unordered_map>
#include <vector>
#include "device.hpp"

namespace AgpuD3D12
{

class ShaderStageInstanceName
{
public:
	ShaderStageInstanceName()
		: stageType(AGPU_LIBRARY_SHADER)
	{}
	ShaderStageInstanceName(const agpu::shader_signature_ref& csignature, agpu_shader_type cstageType, const std::string& centryPointName)
		: signature(csignature), stageType(cstageType), entryPointName(centryPointName)
	{}

	bool operator==(const ShaderStageInstanceName& o) const
	{
		return signature == o.signature && stageType == o.stageType && entryPointName == o.entryPointName;
	}

	size_t hash() const
	{
		return signature.hash() ^ std::hash<agpu_shader_type>() (stageType) ^ std::hash<std::string>()(entryPointName);
	}

	agpu::shader_signature_weakref signature;
	agpu_shader_type stageType;
	std::string entryPointName;
};

}

namespace std
{
template<>
struct hash<AgpuD3D12::ShaderStageInstanceName>
{
	size_t operator()(const AgpuD3D12::ShaderStageInstanceName& name) const
	{
		return name.hash();
	}
};
} // End of namespace std

namespace AgpuD3D12
{
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
    agpu_error getShaderBytecodeForEntryPoint(const agpu::shader_signature_ref &shaderSignature, agpu_shader_type type, const std::string & entryPoint, std::string& outCompilationLog, D3D12_SHADER_BYTECODE *out);
	agpu_error getConvertedSpirVBytecodeForEntryPoint(const agpu::shader_signature_ref& shaderSignature, agpu_shader_type type, const std::string& entryPoint, std::string& outCompilationLog, D3D12_SHADER_BYTECODE* out);
	agpu_error convertSpirVIntoBytecode(const agpu::shader_signature_ref& shaderSignature, agpu_shader_type type, const std::string& entryPoint, std::string& outCompilationLog);

    agpu_shader_type type;
    agpu_shader_language shaderLanguage;
    agpu::device_ref device;

    std::vector<char> sourceCode;
    std::vector<char> objectCode;
    std::string compilationLog;

private:
	std::unordered_map<ShaderStageInstanceName, std::vector<char>> shaderStageInstances;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_SHADER_HPP
