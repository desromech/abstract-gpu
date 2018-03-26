#include "shader.hpp"
#include "shader_signature.hpp"

static int shaderDumpCount = 0;

static spv::ExecutionModel mapShaderTypeIntoExecutionModel(agpu_shader_type type)
{
    switch(type)
    {
    default:
    case AGPU_VERTEX_SHADER: return spv::ExecutionModelVertex;
    case AGPU_FRAGMENT_SHADER: return spv::ExecutionModelFragment;
    case AGPU_COMPUTE_SHADER: return spv::ExecutionModelGLCompute;
    
    }
}
agpu_shader_forSignature::agpu_shader_forSignature()
{
    library = nil;
    function = nil;
}

void agpu_shader_forSignature::lostReferences()
{
    if(function)
        [function release];
    if(library)
        [library release];
}

agpu_error agpu_shader_forSignature::compile(std::string *errorMessage, agpu_cstring options)
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

agpu_error agpu_shader_forSignature::compileMetalSource ( std::string *errorMessage, agpu_cstring options )
{
    auto sourceString = [[NSString alloc] initWithBytes: &source[0] length: source.size() encoding: NSUTF8StringEncoding];
    if(!sourceString)
    {
        *errorMessage = "Missing source code.";
        return AGPU_ERROR;
    }

    NSError *error = nil;
    auto compileOptions = [[MTLCompileOptions alloc] init];
    library = [device->device newLibraryWithSource: sourceString options: compileOptions error: &error];
    [sourceString release];
    if(!library)
    {
        auto description = [error localizedDescription];
        *errorMessage = [description UTF8String];
        return AGPU_COMPILATION_ERROR;
    }

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

agpu_error agpu_shader_forSignature::compileMetalBlob ( std::string *errorMessage, agpu_cstring options )
{
    return AGPU_UNSUPPORTED;
}

_agpu_shader::_agpu_shader(agpu_device *device)
    : device(device)
{
    genericShaderInstance = nullptr;
}

void _agpu_shader::lostReferences()
{
    if(genericShaderInstance)
        genericShaderInstance->release();
}

agpu_shader *_agpu_shader::create(agpu_device* device, agpu_shader_type type)
{
    switch(type)
    {
    case AGPU_GEOMETRY_SHADER:
    case AGPU_TESSELLATION_CONTROL_SHADER:
    case AGPU_TESSELLATION_EVALUATION_SHADER:
        return nullptr;
    default:
        // Ignore, they are supported.
        break;
    }

    auto result = new agpu_shader(device);
    result->type = type;
    return result;
}

agpu_error _agpu_shader::setSource ( agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
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

agpu_error _agpu_shader::compile ( agpu_cstring options )
{
    if(source.empty())
        return AGPU_INVALID_OPERATION;
    if(language == AGPU_SHADER_LANGUAGE_METAL || language == AGPU_SHADER_LANGUAGE_METAL_AIR)
    {
        auto result = new agpu_shader_forSignature();
        result->device = device;
        result->type = type;
        result->language = language;
        result->source = source;
        
        auto error = result->compile(&compilationLog, options);
        if(error)
        {
            result->release();
            return error;
        }
        
        if(genericShaderInstance)
            genericShaderInstance->release();
        genericShaderInstance = result;
        return AGPU_OK;
    }
    else
    {
        return AGPU_OK;
    }
}

agpu_size _agpu_shader::getCompilationLogLength (  )
{
    return compilationLog.size();
}

agpu_error _agpu_shader::getCompilationLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(buffer);
    memcpy(buffer, compilationLog.data(), std::min((size_t)buffer_size, compilationLog.size()));
    return AGPU_OK;
}

agpu_error _agpu_shader::getOrCreateShaderInstanceForSignature(agpu_shader_signature *signature, agpu_uint vertexBufferCount, const std::string &entryPoint, std::string *errorMessage, agpu_shader_forSignature **result)
{
    if(language == AGPU_SHADER_LANGUAGE_METAL || language == AGPU_SHADER_LANGUAGE_METAL_AIR)
    {
        genericShaderInstance->retain();
        *result = genericShaderInstance;
        return AGPU_OK;
    }
    else if(language == AGPU_SHADER_LANGUAGE_SPIR_V)
        return getOrCreateSpirVShaderInstanceForSignature(signature, vertexBufferCount, entryPoint, errorMessage, result);
    else
        return AGPU_UNSUPPORTED;
}

agpu_error _agpu_shader::getOrCreateSpirVShaderInstanceForSignature(agpu_shader_signature *signature, agpu_uint vertexBufferCount, const std::string &expectedEntryPointName, std::string *errorMessage, agpu_shader_forSignature **result)
{
    char buffer[256];
    uint32_t *rawData = reinterpret_cast<uint32_t *> (&source[0]);
	size_t rawDataSize = source.size() / 4;

    auto resourceBindings = signature->resourceBindings;
    auto expectedExecutionModel = mapShaderTypeIntoExecutionModel(type);
    for(auto &binding : resourceBindings)
    {
        binding.stage = expectedExecutionModel;
        if(type == AGPU_VERTEX_SHADER)
        {
            binding.msl_buffer += vertexBufferCount;
        }
    }

    //printf("getOrCreateSpirVShaderInstanceForSignature\n");
	spirv_cross::CompilerMSL msl(rawData, rawDataSize, nullptr, 0, &resourceBindings[0], resourceBindings.size());
    
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
        *errorMessage = "Shader does not have a valid entry point";
        return AGPU_COMPILATION_ERROR;
    }
    
    msl.set_entry_point(usedEntryPoint, expectedExecutionModel);

	// Modify the resources
	spirv_cross::ShaderResources resources = msl.get_shader_resources();

	// Set some options.
	spirv_cross::CompilerMSL::Options options;
	msl.set_options(options);

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
	auto shaderInstance = new agpu_shader_forSignature();
	shaderInstance->device = device;
	shaderInstance->type = type;
    shaderInstance->language = AGPU_SHADER_LANGUAGE_METAL;
	shaderInstance->source = std::vector<uint8_t> ((uint8_t*)&compiled[0], (uint8_t*)&compiled[0] + compiled.size());
    shaderInstance->entryPoint = msl.get_cleansed_entry_point_name(usedEntryPoint, expectedExecutionModel);
    
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
    {
        shaderInstance->release();
        return error;
    }
    
    *result = shaderInstance;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderReference ( agpu_shader* shader )
{
    CHECK_POINTER(shader);
    return shader->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShader ( agpu_shader* shader )
{
    CHECK_POINTER(shader);
    return shader->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSource ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
{
    CHECK_POINTER(shader);
    return shader->setSource(language, sourceText, sourceTextLength);
}

AGPU_EXPORT agpu_error agpuCompileShader ( agpu_shader* shader, agpu_cstring options )
{
    CHECK_POINTER(shader);
    return shader->compile(options);
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength ( agpu_shader* shader )
{
    if(!shader)
        return 0;
    return shader->getCompilationLogLength();
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(shader);
    return shader->getCompilationLog(buffer_size, buffer);
}
