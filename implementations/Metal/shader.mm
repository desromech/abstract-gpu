#include "shader.hpp"

_agpu_shader::_agpu_shader(agpu_device *device)
    : device(device)
{
    library = nil;
    function = nil;
}

void _agpu_shader::lostReferences()
{
    if(function)
        [function release];
    if(library)
        [library release];
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
    if(language != AGPU_SHADER_LANGUAGE_METAL && language != AGPU_SHADER_LANGUAGE_METAL_AIR)
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
    if(language == AGPU_SHADER_LANGUAGE_METAL)
        return compileMetalSource(options);
    else
        return compileMetalBlob(options);
}

agpu_error _agpu_shader::compileMetalSource ( agpu_cstring options )
{
    auto sourceString = [[NSString alloc] initWithBytes: &source[0] length: source.size() encoding: NSUTF8StringEncoding];
    if(!sourceString)
        return AGPU_ERROR;

    NSError *error = nil;
    auto compileOptions = [[MTLCompileOptions alloc] init];
    library = [device->device newLibraryWithSource: sourceString options: compileOptions error: &error];
    [sourceString release];
    if(!library)
    {
        auto description = [error localizedDescription];
        compilationLog = [description UTF8String];
        return AGPU_COMPILATION_ERROR;
    }

    function = [library newFunctionWithName: @"agpu_main"];
    if(!function)
        function = [library newFunctionWithName: @"shaderMain"];

    if(!function)
    {
        compilationLog = "Missing agpu_main entry point.";
        return AGPU_COMPILATION_ERROR;
    }

    return AGPU_OK;
}

agpu_error _agpu_shader::compileMetalBlob ( agpu_cstring options )
{
    return AGPU_UNIMPLEMENTED;
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
