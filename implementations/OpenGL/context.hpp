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

    virtual agpu_error setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h ) = 0;
    virtual agpu_error setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h ) = 0;

    virtual agpu_error setClearColor ( agpu_float r, agpu_float g, agpu_float b, agpu_float a ) = 0;
    virtual agpu_error setClearDepth ( agpu_float depth ) = 0;
    virtual agpu_error setClearStencil ( agpu_int value ) = 0;

    virtual agpu_error clear ( agpu_bitfield buffers ) = 0;

    virtual agpu_error setDepthFunction ( agpu_compare_function function ) = 0;
    virtual agpu_error setAlphaFunction ( agpu_compare_function function, agpu_float reference ) = 0;

    virtual agpu_error uploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data ) = 0;

    virtual agpu_error useProgram (agpu_program* program ) = 0;
    virtual agpu_error useVertexBinding ( agpu_vertex_binding* vertex_binding ) = 0;
    virtual agpu_error useIndexBuffer ( agpu_buffer* index_buffer ) = 0;
    virtual agpu_error useDrawBuffer ( agpu_buffer* draw_buffer ) = 0;
    
    virtual agpu_error drawElementsIndirect ( agpu_primitive_mode mode, agpu_size offset ) = 0;
    virtual agpu_error multiDrawElementsIndirect ( agpu_primitive_mode mode, agpu_size offset, agpu_size drawcount ) = 0;
    
    virtual agpu_error setUniform1i ( agpu_int location, agpu_size count, agpu_int* data ) = 0;
    virtual agpu_error setUniform2i ( agpu_int location, agpu_size count, agpu_int* data ) = 0;
    virtual agpu_error setUniform3i ( agpu_int location, agpu_size count, agpu_int* data ) = 0;
    virtual agpu_error setUniform4i ( agpu_int location, agpu_size count, agpu_int* data ) = 0;
    virtual agpu_error setUniform1f ( agpu_int location, agpu_size count, agpu_float* data ) = 0;
    virtual agpu_error setUniform2f ( agpu_int location, agpu_size count, agpu_float* data ) = 0;
    virtual agpu_error setUniform3f ( agpu_int location, agpu_size count, agpu_float* data ) = 0;
    virtual agpu_error setUniform4f ( agpu_int location, agpu_size count, agpu_float* data ) = 0;
    virtual agpu_error setUniformMatrix2f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data ) = 0;
    virtual agpu_error setUniformMatrix4f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data ) = 0;
    virtual agpu_error setUniformMatrix3f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data ) = 0;
};

class AgpuGLImmediateContext: public AgpuGLContext
{
public:
    AgpuGLImmediateContext(_agpu_device *device);
    ~AgpuGLImmediateContext();

    virtual agpu_error makeCurrent();
    virtual agpu_error finish();
    virtual agpu_error flush();

    virtual agpu_error setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h );
    virtual agpu_error setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h );

    virtual agpu_error setClearColor ( agpu_float r, agpu_float g, agpu_float b, agpu_float a );
    virtual agpu_error setClearDepth ( agpu_float depth );
    virtual agpu_error setClearStencil ( agpu_int value );

    virtual agpu_error clear ( agpu_bitfield buffers );

    virtual agpu_error setDepthFunction ( agpu_compare_function function );
    virtual agpu_error setAlphaFunction ( agpu_compare_function function, agpu_float reference );

    virtual agpu_error uploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

    virtual agpu_error useProgram (agpu_program* program );
    virtual agpu_error useVertexBinding ( agpu_vertex_binding* vertex_binding );
    virtual agpu_error useIndexBuffer ( agpu_buffer* index_buffer );
    virtual agpu_error useDrawBuffer ( agpu_buffer* draw_buffer );
    
    virtual agpu_error drawElementsIndirect ( agpu_primitive_mode mode, agpu_size offset );
    virtual agpu_error multiDrawElementsIndirect ( agpu_primitive_mode mode, agpu_size offset, agpu_size drawcount );
    
    virtual agpu_error setUniform1i ( agpu_int location, agpu_size count, agpu_int* data );
    virtual agpu_error setUniform2i ( agpu_int location, agpu_size count, agpu_int* data );
    virtual agpu_error setUniform3i ( agpu_int location, agpu_size count, agpu_int* data );
    virtual agpu_error setUniform4i ( agpu_int location, agpu_size count, agpu_int* data );
    virtual agpu_error setUniform1f ( agpu_int location, agpu_size count, agpu_float* data );
    virtual agpu_error setUniform2f ( agpu_int location, agpu_size count, agpu_float* data );
    virtual agpu_error setUniform3f ( agpu_int location, agpu_size count, agpu_float* data );
    virtual agpu_error setUniform4f ( agpu_int location, agpu_size count, agpu_float* data );
    virtual agpu_error setUniformMatrix2f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data );
    virtual agpu_error setUniformMatrix3f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data );
    virtual agpu_error setUniformMatrix4f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data );
    
    agpu_device *device;
    
public:
    agpu_vertex_binding *currentVertexBinding;
    agpu_buffer *currentIndexBuffer;
    agpu_buffer *currentDrawBuffer;
};

class _agpu_context: public Object<_agpu_context>
{
public:
    _agpu_context(AgpuGLContext *impl);

    void lostReferences();

    AgpuGLContext *impl;
};

#endif // _AGPU_GL_CONTEXT_HPP
