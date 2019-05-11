#ifndef AGPU_OFFLINE_SHADER_COMPILER_HPP
#define AGPU_OFFLINE_SHADER_COMPILER_HPP

#include <AGPU/agpu_impl.hpp>
#include <vector>
#include <string>

namespace AgpuCommon
{

class GLSLangOfflineShaderCompiler : public agpu::offline_shader_compiler
{
public:
    GLSLangOfflineShaderCompiler();
    ~GLSLangOfflineShaderCompiler();

    static agpu::offline_shader_compiler_ref create();
    static agpu::offline_shader_compiler_ref createForDevice(const agpu::device_ref &device);

    virtual agpu_bool isShaderLanguageSupported(agpu_shader_language language) override;
	virtual agpu_bool isTargetShaderLanguageSupported(agpu_shader_language language) override;
	virtual agpu_error setShaderSource(agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength) override;
	virtual agpu_error compileShader(agpu_shader_language target_language, agpu_cstring options) override;
	virtual agpu_size getCompilationLogLength() override;
	virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) override;
    virtual agpu_size getCompilationResultLength() override;
	virtual agpu_error getCompilationResult(agpu_size buffer_size, agpu_string_buffer buffer) override;
	virtual agpu::shader_ptr getResultAsShader() override;

    agpu_error compileShaderWithGLSLang(agpu_shader_language target_language, agpu_cstring options);
    agpu_error assembleSpirVSource(agpu_shader_language target_language, agpu_cstring options);
    agpu_error createDeviceSpecificShader();

    agpu::device_ref device;

    agpu_shader_language shaderLanguage;
    agpu_shader_type shaderStage;
    std::vector<char> shaderSource;
    std::string compilationLog;

    std::vector<uint32_t> spirvCode;
    agpu::shader_ref shaderHandleResult;

};
} // End of namespace AgpuCommon

#endif //AGPU_OFFLINE_SHADER_COMPILER_HPP
