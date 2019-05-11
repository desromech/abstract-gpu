#include "offline_shader_compiler.hpp"
#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"
#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/GLSL.std.450.h"
//#include "SPIRV/doc.h"
//#include "SPIRV/disassemble.h"

#include <string.h>
#include <mutex>

namespace AgpuCommon
{

static std::once_flag shaderCompilerLibraryInitializedFlag;

static inline EShLanguage mapShaderStage(agpu_shader_type stage)
{
    switch(stage)
    {
    case AGPU_VERTEX_SHADER: return EShLangVertex;
    case AGPU_FRAGMENT_SHADER: return EShLangFragment;
    case AGPU_TESSELLATION_CONTROL_SHADER: return EShLangTessControl;
    case AGPU_TESSELLATION_EVALUATION_SHADER: return EShLangTessEvaluation;
    case AGPU_GEOMETRY_SHADER: return EShLangGeometry;
    case AGPU_COMPUTE_SHADER: return EShLangCompute;
    default: return EShLangCount;
    }
}

static inline glslang::EShSource mapShaderSourceLanguage(agpu_shader_language language)
{
    switch(language)
    {
    case AGPU_SHADER_LANGUAGE_GLSL: return glslang::EShSourceGlsl;
    case AGPU_SHADER_LANGUAGE_EGLSL: return glslang::EShSourceGlsl;
    case AGPU_SHADER_LANGUAGE_VGLSL: return glslang::EShSourceGlsl;
    case AGPU_SHADER_LANGUAGE_HLSL: return glslang::EShSourceHlsl;
    default: return glslang::EShSourceNone;
    }
}

static inline glslang::EShClient mapShaderSourceClient(agpu_shader_language language)
{
    switch(language)
    {
    default:
    case AGPU_SHADER_LANGUAGE_GLSL: return glslang::EShClientOpenGL;
    case AGPU_SHADER_LANGUAGE_EGLSL: return glslang::EShClientOpenGL;
    case AGPU_SHADER_LANGUAGE_VGLSL: return glslang::EShClientVulkan;
    case AGPU_SHADER_LANGUAGE_HLSL: return glslang::EShClientVulkan;
    }
}

GLSLangOfflineShaderCompiler::GLSLangOfflineShaderCompiler()
{
}

GLSLangOfflineShaderCompiler::~GLSLangOfflineShaderCompiler()
{
}

agpu::offline_shader_compiler_ref GLSLangOfflineShaderCompiler::create()
{
    return agpu::makeObject<GLSLangOfflineShaderCompiler> ();
}

agpu::offline_shader_compiler_ref GLSLangOfflineShaderCompiler::createForDevice(const agpu::device_ref &device)
{
    auto result = agpu::makeObject<GLSLangOfflineShaderCompiler> ();
    auto compiler = result.as<GLSLangOfflineShaderCompiler> ();
    compiler->device = device;
    return result;
}

agpu_bool GLSLangOfflineShaderCompiler::isShaderLanguageSupported(agpu_shader_language language)
{
    switch(language)
    {
    case AGPU_SHADER_LANGUAGE_GLSL: return true;
    case AGPU_SHADER_LANGUAGE_EGLSL: return true;
    case AGPU_SHADER_LANGUAGE_VGLSL: return true;
    case AGPU_SHADER_LANGUAGE_HLSL: return true;
    case AGPU_SHADER_LANGUAGE_SPIR_VASSEMBLY: return true;
    default: return false;
    }
}

agpu_bool GLSLangOfflineShaderCompiler::isTargetShaderLanguageSupported(agpu_shader_language language)
{
    switch(language)
    {
    case AGPU_SHADER_LANGUAGE_DEVICE_SHADER: return (bool)device;
    case AGPU_SHADER_LANGUAGE_SPIR_V: return true;
    default: return false;
    }
}

agpu_error GLSLangOfflineShaderCompiler::setShaderSource(agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength)
{
    if(!sourceText)
        return AGPU_NULL_POINTER;
    if(!isShaderLanguageSupported(language))
        return AGPU_UNSUPPORTED;
    if(stage == AGPU_LIBRARY_SHADER)
        return AGPU_UNSUPPORTED;

    if(sourceTextLength < 0)
		sourceTextLength = (agpu_string_length)strlen(sourceText);

    this->shaderLanguage = language;
    this->shaderStage = stage;
    this->shaderSource.resize(sourceTextLength);
    memcpy(&this->shaderSource[0], sourceText, sourceTextLength);
    return AGPU_OK;
}

agpu_error GLSLangOfflineShaderCompiler::compileShader(agpu_shader_language target_language, agpu_cstring options)
{
    switch(shaderLanguage)
    {
    case AGPU_SHADER_LANGUAGE_SPIR_VASSEMBLY:
        return assembleSpirVSource(target_language, options);
    case AGPU_SHADER_LANGUAGE_GLSL:
    case AGPU_SHADER_LANGUAGE_EGLSL:
    case AGPU_SHADER_LANGUAGE_VGLSL:
    case AGPU_SHADER_LANGUAGE_HLSL:
        return compileShaderWithGLSLang(target_language, options);
    default:
        return AGPU_UNSUPPORTED;
    }
}

agpu_error GLSLangOfflineShaderCompiler::compileShaderWithGLSLang(agpu_shader_language target_language, agpu_cstring options)
{
    if(shaderSource.empty())
        return AGPU_INVALID_OPERATION;

    if(!isTargetShaderLanguageSupported(target_language))
        return AGPU_UNSUPPORTED;

    // Initialize the shader compiler library.
    std::call_once(shaderCompilerLibraryInitializedFlag, []{
        glslang::InitializeProcess();
    });

    // Create the shader compiler.
    auto glslStage = mapShaderStage(shaderStage);
    glslang::TShader shader(glslStage);

    // Set the shader source string.
    auto shaderSourceStringPointer = &shaderSource[0];
    int shaderSourceStringLength = shaderSource.size();
    shader.setStringsWithLengths(&shaderSourceStringPointer, &shaderSourceStringLength, 1);

    // Setup the shader compiler.
    shader.setEnvInput(mapShaderSourceLanguage(shaderLanguage), glslStage, mapShaderSourceClient(shaderLanguage), 10);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

    // Parse the shader source.
    auto errorMessage = EShMsgSpvRules | EShMsgVulkanRules;
    auto parseResult = shader.parse(&glslang::DefaultTBuiltInResource, 450, ECoreProfile, false, false, EShMessages(errorMessage));

    // Retrieve the content from the info log.
    compilationLog = shader.getInfoLog();
    if(!parseResult)
        return AGPU_COMPILATION_ERROR;

    // Create the program.
    glslang::TProgram program;
    program.addShader(&shader);

    // Link the shader.
    auto linkResult = program.link(EShMessages(errorMessage));
    compilationLog += program.getInfoLog();
    if(!linkResult)
        return AGPU_COMPILATION_ERROR;

    // Apply the IO mapping.
    linkResult = program.mapIO();
    if(!linkResult)
        return AGPU_COMPILATION_ERROR;

    // Get IR of the shader stage.
    auto ir = program.getIntermediate(glslStage);
    if(!ir)
    {
        compilationLog += "Failed to retrieve glslang stage IR.";
        return AGPU_COMPILATION_ERROR;
    }

    // Generate the Spir-V code.
    std::string warningsErrors;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    spvOptions.generateDebugInfo = true;
    spvOptions.disableOptimizer = false;
    spvOptions.optimizeSize = false;
    spvOptions.disassemble = false;
    spvOptions.validate = false;
    glslang::GlslangToSpv(*ir, spirvCode, &logger, &spvOptions);

    compilationLog += logger.getAllMessages();
    if(target_language == AGPU_SHADER_LANGUAGE_DEVICE_SHADER)
        return createDeviceSpecificShader();

    return AGPU_OK;
}

agpu_error GLSLangOfflineShaderCompiler::assembleSpirVSource(agpu_shader_language target_language, agpu_cstring options)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error GLSLangOfflineShaderCompiler::createDeviceSpecificShader()
{
    // Create the device shader.
    auto deviceShader = agpu::shader_ref(device->createShader(shaderStage));
    if(!deviceShader)
    {
        compilationLog += "Failed to create device specific shader stage object.\n";
        return AGPU_COMPILATION_ERROR;
    }

    // Set the shader source.
    auto error = deviceShader->setShaderSource(AGPU_SHADER_LANGUAGE_SPIR_V, reinterpret_cast<agpu_cstring> (&spirvCode[0]), spirvCode.size()*4);
    if(error) return error;

    // Compile the device shader source.
    error = deviceShader->compileShader(nullptr);

    // Append the device shader compilation log.
    auto deviceShaderLogLength = deviceShader->getCompilationLogLength();
    if(deviceShaderLogLength > 0)
    {
        std::unique_ptr<char []> buffer(new char[deviceShaderLogLength + 1]);
        deviceShader->getCompilationLog(deviceShaderLogLength + 1, buffer.get());

        compilationLog += "Device shader compilation log:\n";
        compilationLog += buffer.get();
    }

    if(error) return error;

    shaderHandleResult = deviceShader;
    return AGPU_OK;
}

agpu_size GLSLangOfflineShaderCompiler::getCompilationLogLength()
{
    return compilationLog.size();
}

agpu_error GLSLangOfflineShaderCompiler::getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    if(!buffer)
        return AGPU_NULL_POINTER;

    if(buffer_size == 0)
		return AGPU_INVALID_PARAMETER;

    size_t toCopy = std::min(size_t(buffer_size - 1), compilationLog.size());
    if(toCopy > 0)
        memcpy(buffer, compilationLog.data(), toCopy);
    buffer[buffer_size-1] = 0;
	return AGPU_OK;
}

agpu_size GLSLangOfflineShaderCompiler::getCompilationResultLength()
{
    return spirvCode.size() * 4;
}

agpu_error GLSLangOfflineShaderCompiler::getCompilationResult(agpu_size buffer_size, agpu_string_buffer buffer)
{
    if(buffer_size != getCompilationResultLength())
        return AGPU_INVALID_PARAMETER;

    if(!spirvCode.empty())
        memcpy(buffer, &spirvCode[0], buffer_size);

    return AGPU_OK;
}

agpu::shader_ptr GLSLangOfflineShaderCompiler::getResultAsShader()
{
    return shaderHandleResult.disownedNewRef();
}

} // End of namespace AgpuCommon
