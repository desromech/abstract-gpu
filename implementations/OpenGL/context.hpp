#ifndef _AGPU_GL_CONTEXT_HPP
#define _AGPU_GL_CONTEXT_HPP

#include "device.hpp"

/**
 * AGPU GL context interface
 */
class AgpuGLContext
{
public:
    virtual ~AgpuGLContext() {}

    virtual agpu_error makeCurrent() = 0;
    virtual agpu_error finish() = 0;
    virtual agpu_error flush() = 0;

    virtual agpu_error setClearColor ( agpu_float r, agpu_float g, agpu_float b, agpu_float a ) = 0;
    virtual agpu_error setClearDepth ( agpu_float depth ) = 0;
    virtual agpu_error setClearStencil ( agpu_int value ) = 0;

    virtual agpu_error clear ( agpu_bitfield buffers ) = 0;

    virtual agpu_error setDepthFunction ( agpu_compare_function function ) = 0;
    virtual agpu_error setAlphaFunction ( agpu_compare_function function, agpu_float reference ) = 0;

    virtual agpu_error uploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data ) = 0;

    virtual agpu_error useProgram (agpu_program* program ) = 0;
};

class AgpuGLImmediateContext: public AgpuGLContext
{
public:
    AgpuGLImmediateContext(_agpu_device *device);
    ~AgpuGLImmediateContext();

    virtual agpu_error makeCurrent();
    virtual agpu_error finish();
    virtual agpu_error flush();

    virtual agpu_error setClearColor ( agpu_float r, agpu_float g, agpu_float b, agpu_float a );
    virtual agpu_error setClearDepth ( agpu_float depth );
    virtual agpu_error setClearStencil ( agpu_int value );

    virtual agpu_error clear ( agpu_bitfield buffers );

    virtual agpu_error setDepthFunction ( agpu_compare_function function );
    virtual agpu_error setAlphaFunction ( agpu_compare_function function, agpu_float reference );

    virtual agpu_error uploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

    virtual agpu_error useProgram (agpu_program* program );

    agpu_device *device;
};

class _agpu_context: public Object<_agpu_context>
{
public:
    _agpu_context(AgpuGLContext *impl);

    void lostReferences();

    AgpuGLContext *impl;
};

#endif // _AGPU_GL_CONTEXT_HPP
