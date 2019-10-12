#include "shader.hpp"
#include "shader_signature.hpp"
#include "spirv_hlsl.hpp"
#include <d3dcompiler.h>
#include <assert.h>

namespace AgpuD3D12
{
inline spv::ExecutionModel mapExecutionModel(agpu_shader_type type)
{
	switch (type)
	{
	case AGPU_VERTEX_SHADER: return spv::ExecutionModelVertex;
	case AGPU_FRAGMENT_SHADER: return spv::ExecutionModelFragment;
	case AGPU_COMPUTE_SHADER: return spv::ExecutionModelGLCompute;
	case AGPU_GEOMETRY_SHADER: return spv::ExecutionModelGeometry;
	case AGPU_TESSELLATION_CONTROL_SHADER: return spv::ExecutionModelTessellationControl;
	case AGPU_TESSELLATION_EVALUATION_SHADER: return spv::ExecutionModelTessellationEvaluation;
	default: abort();
	}
}

static const char* getHlslTarget(agpu_shader_type stageType)
{
	switch (stageType)
	{
	case AGPU_VERTEX_SHADER:
		return "vs_5_1";
	case AGPU_FRAGMENT_SHADER:
		return "ps_5_1";
	case AGPU_GEOMETRY_SHADER:
		return "gs_5_1";
	case AGPU_COMPUTE_SHADER:
		return "cs_5_1";
	case AGPU_TESSELLATION_CONTROL_SHADER:
		return "hs_5_1";
	case AGPU_TESSELLATION_EVALUATION_SHADER:
		return "ds_5_1";
	default:
		abort();
	}
}
static agpu_error compileHlslShader(const agpu::device_ref& device, agpu_shader_type stageType, agpu_cstring options, size_t sourceCodeSize, const char *sourceCode, std::vector<char>& objectCode, std::string& outErrorMessage)
{
	UINT compileFlags = 0;
	if (deviceForDX->isDebugEnabled)
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;
	auto res = D3DCompile(sourceCode, sourceCodeSize, "agpuShader", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", getHlslTarget(stageType), compileFlags, 0, &shaderBlob, &errorBlob);
	if (FAILED(res))
	{
		if (errorBlob)
		{
			const char* buffer = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());
			outErrorMessage += std::string(buffer, buffer + errorBlob->GetBufferSize());
			return AGPU_COMPILATION_ERROR;
		}
	}

	// Success
	const char* rawObject = reinterpret_cast<const char*> (shaderBlob->GetBufferPointer());
	objectCode = std::vector<char>(rawObject, rawObject + shaderBlob->GetBufferSize());

	return AGPU_OK;
}

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
    if (language != AGPU_SHADER_LANGUAGE_HLSL &&
		language != AGPU_SHADER_LANGUAGE_SPIR_V &&
		language != AGPU_SHADER_LANGUAGE_BINARY)
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
	memcpy(&sourceCode[0], sourceText, sourceTextLength);

	return AGPU_OK;
}

agpu_error ADXShader::compileShader(agpu_cstring options)
{
	switch (shaderLanguage)
	{
	case AGPU_SHADER_LANGUAGE_BINARY:
	case AGPU_SHADER_LANGUAGE_SPIR_V:
		return AGPU_OK;
	case AGPU_SHADER_LANGUAGE_HLSL:
		return compileHlslShader(device, type, options, sourceCode.size(), &sourceCode[0], objectCode, compilationLog);
	default:
		compilationLog = "Unsupported shader language.";
		return AGPU_UNSUPPORTED;
	}


	return AGPU_UNSUPPORTED;
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

agpu_error ADXShader::getShaderBytecodeForEntryPoint(const agpu::shader_signature_ref& shaderSignature, agpu_shader_type type, const std::string& entryPoint, std::string& outCompilationLog, D3D12_SHADER_BYTECODE* out)
{
	if (shaderLanguage == AGPU_SHADER_LANGUAGE_SPIR_V)
		return getConvertedSpirVBytecodeForEntryPoint(shaderSignature, type, entryPoint, outCompilationLog, out);

	out->pShaderBytecode = &objectCode[0];
	out->BytecodeLength = objectCode.size();
	return AGPU_OK;
}

agpu_error ADXShader::getConvertedSpirVBytecodeForEntryPoint(const agpu::shader_signature_ref& shaderSignature, agpu_shader_type type, const std::string& entryPoint, std::string& outCompilationLog, D3D12_SHADER_BYTECODE* out)
{
	auto instanceName = ShaderStageInstanceName(shaderSignature, type, entryPoint);
	auto it = shaderStageInstances.find(instanceName);
	if (it == shaderStageInstances.end())
	{
		auto error = convertSpirVIntoBytecode(shaderSignature, type, entryPoint, outCompilationLog);
		if (error) return error;
		it = shaderStageInstances.find(instanceName);
	}
	assert(it != shaderStageInstances.end());

	auto& objectCode = it->second;
	out->pShaderBytecode = &objectCode[0];
	out->BytecodeLength = objectCode.size();
	return AGPU_OK;
}

static agpu_error mapShaderResources(spirv_cross::CompilerHLSL& compiler,
	std::vector<spirv_cross::Resource>& resources, const agpu::shader_signature_ref& signature, std::string& errorMessage, agpu_shader_binding_type bindingType)
{
	auto adxSignature = signature.as<ADXShaderSignature>();
	for (auto& resource : resources)
	{
		unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

		if (set >= adxSignature->banks.size())
		{
			errorMessage += "Descriptor set used by the shader is out of bounds.\n";
			return AGPU_INVALID_PARAMETER;
		}

		const auto& bank = adxSignature->banks[set];
		if (binding >= bank.elements.size())
		{
			errorMessage += "Descriptor binding in set used by the shader is out of bounds.\n";
			return AGPU_INVALID_PARAMETER;
		}

		int mappedBinding = bank.elements[binding].baseDescriptorIndex;

		//printf("Resource %s set %d binding %d -> %d\n", resource.name.c_str(), set, binding, mappedBinding);
		compiler.set_decoration(resource.id, spv::DecorationBinding, mappedBinding);
	}

	return AGPU_OK;
}

agpu_error ADXShader::convertSpirVIntoBytecode(const agpu::shader_signature_ref& shaderSignature, agpu_shader_type stageType, const std::string& entryPoint, std::string& errorMessage)
{
	uint32_t* rawData = reinterpret_cast<uint32_t*> (&sourceCode[0]);
	size_t rawDataSize = sourceCode.size() / 4;

	spirv_cross::CompilerHLSL hlsl(rawData, rawDataSize);

	hlsl.set_entry_point(entryPoint, mapExecutionModel(stageType));

	// Modify the resources
	spirv_cross::ShaderResources resources = hlsl.get_shader_resources();

	// Uniform buffers
	auto error = mapShaderResources(hlsl, resources.uniform_buffers, shaderSignature, errorMessage, AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER);
	if (error) return error;

	// Storage buffers
	error = mapShaderResources(hlsl, resources.storage_buffers, shaderSignature, errorMessage, AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER);
	if (error) return error;

	// Samplers
	error = mapShaderResources(hlsl, resources.separate_samplers, shaderSignature, errorMessage, AGPU_SHADER_BINDING_TYPE_SAMPLER);
	if (error) return error;

	// Sampled images
	error = mapShaderResources(hlsl, resources.separate_images, shaderSignature, errorMessage, AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE);
	if (error) return error;

	// Storage images
	error = mapShaderResources(hlsl, resources.storage_images, shaderSignature, errorMessage, AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE);
	if (error) return error;

	// Does the shader signature have push constants?
	auto adxShaderSignature = shaderSignature.as<ADXShaderSignature>();
	if (adxShaderSignature->pushConstantCount > 0)
	{
		std::vector<spirv_cross::RootConstants> pushConstantsLayout;
		spirv_cross::RootConstants pushConstants = {};
		pushConstants.start = 0;
		pushConstants.end = adxShaderSignature->pushConstantCount*4;
		pushConstants.space = adxShaderSignature->banks.size();
		pushConstants.binding = 0;
		pushConstantsLayout.push_back(pushConstants);
		hlsl.set_root_constant_layouts(pushConstantsLayout);
	}

	// Set some options.
	spirv_cross::CompilerHLSL::Options options;
	options.shader_model = 51;
	hlsl.set_hlsl_options(options);

	// Compile the shader.
	std::string compiled;
	try
	{
		compiled = hlsl.compile();
		//printf("Compiled spirv -> hlsl:\n%s\n", compiled.c_str());
	}
	catch (spirv_cross::CompilerError& compileError)
	{
		errorMessage += "Failed to convert Spir-V into HLSL\n";
		errorMessage += compileError.what();
		return AGPU_COMPILATION_ERROR;
	}

	// Compile the hlsl source.
	std::vector<char> instanceObjectCode;
	error = compileHlslShader(device, stageType, "", compiled.size(), compiled.data(), instanceObjectCode, errorMessage);
	if (error)
	{
		errorMessage += "HLSL Shader source code generated from Spir-V shader:\n";
		errorMessage += compiled;
		errorMessage += "\n";
		return error;
	}

	// Store the instance object code.
	shaderStageInstances.insert(std::make_pair(ShaderStageInstanceName(shaderSignature, stageType, entryPoint), instanceObjectCode));

	return AGPU_OK;
}

} // End of namespace AgpuD3D12
