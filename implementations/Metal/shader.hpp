#ifndef AGPU_METAL_SHADER_HPP
#define AGPU_METAL_SHADER_HPP

#include "device.hpp"
#include <string>
#include <vector>

namespace AgpuMetal
{
    
class AMtlShaderForSignature: public agpu::base_interface
{
public:
	typedef AMtlShaderForSignature main_interface;
    
    AMtlShaderForSignature();
    ~AMtlShaderForSignature();

    agpu_error compile(std::string *errorMessage, agpu_cstring options);
    agpu_error compileMetalSource ( std::string *errorMessage, agpu_cstring options );
    agpu_error compileMetalBlob ( std::string *errorMessage, agpu_cstring options );

    agpu::device_ref device;
    agpu_shader_type type;
    agpu_shader_language language;

    std::vector<uint8_t> source;
    std::string entryPoint;
    id<MTLLibrary> library;
    id<MTLFunction> function;
    MTLSize localSize; // Only used by compute
};

typedef agpu::ref<AMtlShaderForSignature> AMtlShaderForSignatureRef;

class AMtlShader : public agpu::shader
{
public:
    AMtlShader(const agpu::device_ref &device);
    ~AMtlShader();

    static agpu::shader_ref create(const agpu::device_ref &device, agpu_shader_type type);
    
    agpu_error getOrCreateShaderInstanceForSignature(const agpu::shader_signature_ref &signature, const std::string &entryPoint, agpu_shader_type entryPointStage, std::string *errorMessage, AMtlShaderForSignatureRef *result);
    agpu_error getOrCreateSpirVShaderInstanceForSignature(const agpu::shader_signature_ref &signature, const std::string &entryPoint, agpu_shader_type entryPointStage, std::string *errorMessage, AMtlShaderForSignatureRef *result);

    virtual agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength) override;
    virtual agpu_error compileShader(agpu_cstring options) override;
    virtual agpu_size getCompilationLogLength() override;
    virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) override;

    agpu::device_ref device;
    agpu_shader_type type;
    agpu_shader_language language;
    std::string compilationLog;
    std::vector<uint8_t> source;
    
    AMtlShaderForSignatureRef genericShaderInstance;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_SHADER_HPP
