#include "shader.hpp"
#include "shader_signature.hpp"

namespace AgpuMetal
{
    
static int shaderDumpCount = 0;

static spv::ExecutionModel mapShaderTypeIntoExecutionModel(agpu_shader_type type)
{
    switch(type)
    {
    default: abort();
    case AGPU_VERTEX_SHADER: return spv::ExecutionModelVertex;
    case AGPU_FRAGMENT_SHADER: return spv::ExecutionModelFragment;
    case AGPU_COMPUTE_SHADER: return spv::ExecutionModelGLCompute;
    
    }
}

static inline bool isEntryPointNameBlacklisted(const std::string &entryPointName)
{
    return entryPointName == "vertex"|| 
        entryPointName == "fragment" ||
        entryPointName == "compute";
}

AMtlShaderForSignature::AMtlShaderForSignature()
{
    library = nil;
    function = nil;
}

AMtlShaderForSignature::~AMtlShaderForSignature()
{
    if(function)
        [function release];
    if(library)
        [library release];
}

agpu_error AMtlShaderForSignature::compile(std::string *errorMessage, agpu_cstring options)
{
    if(language == AGPU_SHADER_LANGUAGE_METAL)
        return compileMetalSource(errorMessage, options);
    else if(language == AGPU_SHADER_LANGUAGE_METAL_AIR)
        return compileMetalBlob(errorMessage, options);
    else
    {
        *errorMessage = "Unsupported shader language";
        return AGPU_UNSUPPORTED;
    }
}

agpu_error AMtlShaderForSignature::compileMetalSource ( std::string *errorMessage, agpu_cstring options )
{
    auto sourceString = [[NSString alloc] initWithBytes: &source[0] length: source.size() encoding: NSUTF8StringEncoding];
    if(!sourceString)
    {
        *errorMessage = "Missing source code.";
        return AGPU_ERROR;
    }

    NSError *error = nil;
    auto compileOptions = [[MTLCompileOptions alloc] init];
    library = [deviceForMetal->device newLibraryWithSource: sourceString options: compileOptions error: &error];
    [sourceString release];
    
    // Always read the error, if there is one.
    if(error)
    {
        auto description = [error localizedDescription];
        *errorMessage = [description UTF8String];
    }
    
    if(!library)
        return AGPU_COMPILATION_ERROR;

    if(!entryPoint.empty())
    {
        auto entryPointString = [[NSString alloc] initWithBytes: &entryPoint[0] length: entryPoint.size() encoding: NSUTF8StringEncoding];
        function = [library newFunctionWithName: entryPointString];
        [entryPointString release];
    }
    if(!function)
        function = [library newFunctionWithName: @"agpu_main"];
    if(!function)
        function = [library newFunctionWithName: @"shaderMain"];

    if(!function)
    {
        *errorMessage = "Missing agpu_main entry point.";
        return AGPU_COMPILATION_ERROR;
    }

    return AGPU_OK;
}

agpu_error AMtlShaderForSignature::compileMetalBlob ( std::string *errorMessage, agpu_cstring options )
{
    return AGPU_UNSUPPORTED;
}

AMtlShader::AMtlShader(const agpu::device_ref &device)
    : device(device)
{
}

AMtlShader::~AMtlShader()
{
}

agpu::shader_ref AMtlShader::create(const agpu::device_ref &device, agpu_shader_type type)
{
    switch(type)
    {
    case AGPU_GEOMETRY_SHADER:
    case AGPU_TESSELLATION_CONTROL_SHADER:
    case AGPU_TESSELLATION_EVALUATION_SHADER:
        return agpu::shader_ref();
    default:
        // Ignore, they are supported.
        break;
    }

    auto result = agpu::makeObject<AMtlShader> (device);
    result.as<AMtlShader> ()->type = type;
    return result;
}

agpu_error AMtlShader::setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
    CHECK_POINTER(sourceText);
    if(language != AGPU_SHADER_LANGUAGE_METAL &&
       language != AGPU_SHADER_LANGUAGE_METAL_AIR &&
       language != AGPU_SHADER_LANGUAGE_SPIR_V)
        return AGPU_UNSUPPORTED;

    if(sourceTextLength < 0)
    {
        if(language != AGPU_SHADER_LANGUAGE_METAL)
            return AGPU_UNSUPPORTED;
        sourceTextLength = strlen(sourceText);
    }

    this->language = language;
    source = std::vector<uint8_t> (sourceText, sourceText + sourceTextLength);
    return AGPU_OK;
}

agpu_error AMtlShader::compileShader(agpu_cstring options)
{
    if(source.empty())
        return AGPU_INVALID_OPERATION;
    if(language == AGPU_SHADER_LANGUAGE_METAL || language == AGPU_SHADER_LANGUAGE_METAL_AIR)
    {
        auto result = agpu::makeObject<AMtlShaderForSignature> ();
        result->device = device;
        result->type = type;
        result->language = language;
        result->source = source;
        
        auto error = result->compile(&compilationLog, options);
        if(error)
            return error;
        
        genericShaderInstance = result;
        return AGPU_OK;
    }
    else
    {
        return AGPU_OK;
    }
}

agpu_size AMtlShader::getCompilationLogLength (  )
{
    return compilationLog.size();
}

agpu_error AMtlShader::getCompilationLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(buffer);
    memcpy(buffer, compilationLog.data(), std::min((size_t)buffer_size, compilationLog.size()));
    return AGPU_OK;
}

agpu_error AMtlShader::getOrCreateShaderInstanceForSignature(const agpu::shader_signature_ref &signature, const std::string &entryPoint, agpu_shader_type expectedEntryPointName, std::string *errorMessage, AMtlShaderForSignatureRef *result)
{
    if(language == AGPU_SHADER_LANGUAGE_METAL || language == AGPU_SHADER_LANGUAGE_METAL_AIR)
    {
        *result = genericShaderInstance;
        return AGPU_OK;
    }

    else if(language == AGPU_SHADER_LANGUAGE_SPIR_V)
        return getOrCreateSpirVShaderInstanceForSignature(signature, entryPoint, expectedEntryPointName, errorMessage, result);
    else
        return AGPU_UNSUPPORTED;
}

agpu_error AMtlShader::getOrCreateSpirVShaderInstanceForSignature(const agpu::shader_signature_ref &signature, const std::string &expectedEntryPointName, agpu_shader_type expectedEntryPointStage, std::string *errorMessage, AMtlShaderForSignatureRef *result)
{
    char buffer[256];
    uint32_t *rawData = reinterpret_cast<uint32_t *> (&source[0]);
	size_t rawDataSize = source.size() / 4;

    auto resourceBindings = signature.as<AMtlShaderSignature> ()->resourceBindings;
    auto expectedExecutionModel = mapShaderTypeIntoExecutionModel(expectedEntryPointStage);
    for(auto &binding : resourceBindings)
    {
        binding.stage = expectedExecutionModel;
    }

    //printf("getOrCreateSpirVShaderInstanceForSignature\n");
    spirv_cross::CompilerMSL msl(rawData, rawDataSize);
    for (auto & binding : resourceBindings)
        msl.add_msl_resource_binding(binding);
    
    // Get the entry point.
    std::string usedEntryPoint;
    for(auto entryPoint : msl.get_entry_points_and_stages())
    {
        if(entryPoint.name == expectedEntryPointName &&
            entryPoint.execution_model == expectedExecutionModel)
        {
            usedEntryPoint = entryPoint.name;
            break;
        }
    }
    
    if(usedEntryPoint.empty())
    {
        *errorMessage = "Shader does not have a valid entry point.";
        return AGPU_COMPILATION_ERROR;
    }
    
    if(isEntryPointNameBlacklisted(usedEntryPoint))
    {
        auto newEntryPointName = "_agpu_entry_" + usedEntryPoint;
        msl.rename_entry_point(usedEntryPoint, newEntryPointName, expectedExecutionModel);
        usedEntryPoint = newEntryPointName;
    }
    
    // Set the entry point.
    msl.set_entry_point(usedEntryPoint, expectedExecutionModel);

    // Set some options.
	spirv_cross::CompilerMSL::Options options;
	msl.set_msl_options(options);
    
    // Use only the active interface variables.
    auto activeInterfaceVariables = msl.get_active_interface_variables();
    msl.set_enabled_interface_variables(activeInterfaceVariables);

	// Compile the shader.
	std::string compiled;
	try
	{
		compiled = msl.compile();
	}
	catch(spirv_cross::CompilerError &compileError)
	{
		*errorMessage = "Failed to convert Spir-V into msl\n";
		*errorMessage += compileError.what();
		return AGPU_COMPILATION_ERROR;
	}

    //printf("Converted shader into Metal:\n%s\n", compiled.c_str());

    // Create the shader instance object
	auto shaderInstance = agpu::makeObject<AMtlShaderForSignature> ();
	shaderInstance->device = device;
	shaderInstance->type = type;
    shaderInstance->language = AGPU_SHADER_LANGUAGE_METAL;
	shaderInstance->source = std::vector<uint8_t> ((uint8_t*)&compiled[0], (uint8_t*)&compiled[0] + compiled.size());
    shaderInstance->entryPoint = msl.get_cleansed_entry_point_name(usedEntryPoint, expectedExecutionModel);
    
    if(expectedEntryPointStage == AGPU_COMPUTE_SHADER)
    {
        MTLSize localSize;
        localSize.width = msl.get_execution_mode_argument(spv::ExecutionModeLocalSize, 0);
        localSize.height = msl.get_execution_mode_argument(spv::ExecutionModeLocalSize, 1);
        localSize.depth = msl.get_execution_mode_argument(spv::ExecutionModeLocalSize, 2);
        shaderInstance->localSize = localSize;
    }
    
	// Compile the shader instance object.
	auto error = shaderInstance->compile(errorMessage, "");
    //printf("Shader compilation error %d: %s\n", error, errorMessage->c_str());
        
    if(getenv("DUMP_SHADERS") ||
		(error != AGPU_OK && getenv("DUMP_SHADERS_ON_ERROR")))
	{
		snprintf(buffer, sizeof(buffer), "dump%d.spv", shaderDumpCount);
		auto f = fopen(buffer, "wb");
		auto res = fwrite(&source[0], source.size(), 1, f);
		fclose(f);
		(void)res;

		snprintf(buffer, sizeof(buffer), "dump%d.msl", shaderDumpCount);
		f = fopen(buffer, "wb");
		res = fwrite(compiled.data(), compiled.size(), 1, f);
		fclose(f);
		(void)res;

		++shaderDumpCount;
	}
    
    if(error)
        return error;
    
    *result = shaderInstance;
    return AGPU_OK;
}

} // End of namespace AgpuMetal
