#ifndef AGPU_PIPELINE_STATE_HPP_
#define AGPU_PIPELINE_STATE_HPP_

#include "device.hpp"

struct _agpu_pipeline_state: public Object<_agpu_pipeline_state>
{
public:
    _agpu_pipeline_state();

    void lostReferences();
    
    agpu_int getUniformLocation ( agpu_cstring name );
    
public:
    
    agpu_device *device;
    GLuint programHandle;
    
    // States
    agpu_bool depthEnabled;
    agpu_bool depthWriteMask;
    GLenum depthFunction;

    // Stencil testing
    bool stencilEnabled;
    int stencilWriteMask;
    int stencilReadMask;
    
    // Alpha testing
    bool alphaTestEnabled;
	GLenum alphaTestFunction;
    
    // Miscellaneous
    int renderTargetCount;
    agpu_primitive_type primitiveType;

public:
    void activate();
    void enableState(bool enabled, GLenum state);
};


#endif //AGPU_PIPELINE_STATE_HPP_