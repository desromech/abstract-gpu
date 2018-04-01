#ifndef AGPU_METAL_SHADER_HPP
#define AGPU_METAL_SHADER_HPP

#include "device.hpp"
#include <string>
#include <vector>

struct agpu_shader_forSignature: public Object<agpu_shader_forSignature>
{
public:
    agpu_shader_forSignature();

    void lostReferences();

    agpu_error compile(std::string *errorMessage, agpu_cstring options);
    agpu_error compileMetalSource ( std::string *errorMessage, agpu_cstring options );
    agpu_error compileMetalBlob ( std::string *errorMessage, agpu_cstring options );

    agpu_device *device;
    agpu_shader_type type;
    agpu_shader_language language;

    std::vector<uint8_t> source;
    std::string entryPoint;
    id<MTLLibrary> library;
    id<MTLFunction> function;
    MTLSize localSize; // Only used by compute
};

struct _agpu_shader : public Object<_agpu_shader>
{
public:
    _agpu_shader(agpu_device *device);
    void lostReferences();

    static agpu_shader *create(agpu_device* device, agpu_shader_type type);
    
    agpu_error getOrCreateShaderInstanceForSignature(agpu_shader_signature *signature, agpu_uint vertexBufferCount, const std::string &entryPoint, std::string *errorMessage, agpu_shader_forSignature **result);
    agpu_error getOrCreateSpirVShaderInstanceForSignature(agpu_shader_signature *signature, agpu_uint vertexBufferCount, const std::string &entryPoint, std::string *errorMessage, agpu_shader_forSignature **result);

    agpu_error setSource ( agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength );
    agpu_error compile ( agpu_cstring options );
    agpu_size getCompilationLogLength (  );
    agpu_error getCompilationLog ( agpu_size buffer_size, agpu_string_buffer buffer );

    agpu_device *device;
    agpu_shader_type type;
    agpu_shader_language language;
    std::string compilationLog;
    std::vector<uint8_t> source;
    
    agpu_shader_forSignature *genericShaderInstance;
};

#endif //AGPU_METAL_SHADER_HPP
