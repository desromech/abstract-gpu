#ifndef AGPU_PIPELINE_BUILDER_HPP_
#define AGPU_PIPELINE_BUILDER_HPP_

#include "device.hpp"

struct _agpu_pipeline_builder: public Object<_agpu_pipeline_builder>
{
public:
    _agpu_pipeline_builder();
    
    static agpu_pipeline_builder *createBuilder(agpu_device *device);

    void lostReferences();

    agpu_pipeline_state* build ();
    
    agpu_error attachShader ( agpu_shader* shader );
    
    agpu_size getBuildingLogLength (  );
    agpu_error getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer );
    agpu_error setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function );
    agpu_error setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask );
    agpu_error setRenderTargetCount ( agpu_int count );
    agpu_error setPrimitiveType(agpu_primitive_type type);
    agpu_error setVertexLayout(agpu_vertex_layout* layout);

    agpu_error setRenderTargetFormat(agpu_uint index, agpu_texture_format format);
    agpu_error setDepthStencilFormat(agpu_texture_format format);

    agpu_error reset();
        
public:
    agpu_device *device;
    GLuint programHandle;
    bool linked;
    
    // States
    agpu_bool depthEnabled;
    agpu_bool depthWriteMask;
    agpu_compare_function depthFunction;

    // Stencil testing
    bool stencilEnabled;
    int stencilWriteMask;
    int stencilReadMask;

    // Miscellaneos
    int renderTargetCount;
    agpu_primitive_type primitiveType;
};


#endif //AGPU_PIPELINE_BUILDER_HPP_