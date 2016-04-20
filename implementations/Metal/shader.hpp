#ifndef AGPU_METAL_SHADER_HPP
#define AGPU_METAL_SHADER_HPP

#include "device.hpp"
#include <string>
#include <vector>

struct _agpu_shader : public Object<_agpu_shader>
{
public:
    _agpu_shader(agpu_device *device);
    void lostReferences();

    static agpu_shader *create(agpu_device* device, agpu_shader_type type);

    agpu_error setSource ( agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength );
    agpu_error compile ( agpu_cstring options );
    agpu_error compileMetalSource ( agpu_cstring options );
    agpu_error compileMetalBlob ( agpu_cstring options );
    agpu_size getCompilationLogLength (  );
    agpu_error getCompilationLog ( agpu_size buffer_size, agpu_string_buffer buffer );

    agpu_device *device;
    agpu_shader_type type;
    agpu_shader_language language;
    std::string compilationLog;
    std::vector<uint8_t> source;
    id<MTLLibrary> library;
    id<MTLFunction> function;
};

#endif //AGPU_METAL_SHADER_HPP
