#include "context.hpp"
#include "buffer.hpp"
#include "vertex_binding.hpp"
#include "program.hpp"

inline GLenum mapPrimitiveMode(agpu_primitive_mode mode)
{
    switch(mode)
    {
	case AGPU_POINTS: return GL_POINTS;
	case AGPU_LINES: return GL_LINES;
	case AGPU_LINES_ADJACENCY: return GL_LINES_ADJACENCY;
	case AGPU_LINE_STRIP: return GL_LINE_STRIP;
	case AGPU_LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
	case AGPU_LINE_LOOP: return GL_LINE_LOOP;
	case AGPU_TRIANGLES: return GL_TRIANGLES;
	case AGPU_TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
	case AGPU_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
	case AGPU_TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
	case AGPU_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
	case AGPU_PATCHES: return GL_PATCHES;
    default: abort();
    }
}

inline GLenum mapIndexType(agpu_size stride)
{
    switch(stride)
    {
    case 1: return GL_UNSIGNED_BYTE;
    case 2: return GL_UNSIGNED_SHORT;
    case 4: return GL_UNSIGNED_INT;
    default: abort();
    }
}

// Immediate context
AgpuGLImmediateContext::AgpuGLImmediateContext(agpu_device *device)
    : device(device)

{
    currentVertexBinding = nullptr;
    currentIndexBuffer = nullptr;
    currentDrawBuffer = nullptr;
}

AgpuGLImmediateContext::~AgpuGLImmediateContext()
{
}

agpu_error AgpuGLImmediateContext::setClearColor ( agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
    glClearColor(r, g, b, a);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::setClearDepth ( agpu_float depth )
{
    glClearDepth(depth);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::setClearStencil ( agpu_int value )
{
    glClearStencil(value);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::clear ( agpu_bitfield buffers )
{
    GLbitfield bits = 0;
    if(buffers & AGPU_COLOR_BUFFER_BIT)
        bits |= GL_COLOR_BUFFER_BIT;
    if(buffers & AGPU_DEPTH_BUFFER_BIT)
        bits |= GL_DEPTH_BUFFER_BIT;
    if(buffers & AGPU_STENCIL_BUFFER_BIT)
        bits |= GL_STENCIL_BUFFER_BIT;
    glClear(bits);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::setDepthFunction ( agpu_compare_function function )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AgpuGLImmediateContext::setAlphaFunction ( agpu_compare_function function, agpu_float reference )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AgpuGLImmediateContext::finish()
{
    glFinish();
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::flush()
{
    glFlush();
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::makeCurrent()
{
    return device->makeCurrent() ? AGPU_OK : AGPU_NOT_CURRENT_CONTEXT;
}

agpu_error AgpuGLImmediateContext::uploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    buffer->uploadBufferData(offset, size, data);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::useProgram (agpu_program* program )
{
    device->glUseProgram(program ? program->handle : 0);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::useVertexBinding ( agpu_vertex_binding* vertex_binding )
{
    currentVertexBinding = vertex_binding;
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::useIndexBuffer ( agpu_buffer* index_buffer )
{
    currentIndexBuffer = index_buffer;
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::useDrawBuffer ( agpu_buffer* draw_buffer )
{
    currentDrawBuffer = draw_buffer;
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::drawElementsIndirect ( agpu_primitive_mode mode, agpu_size offset )
{
    if(!currentVertexBinding || !currentIndexBuffer || !currentDrawBuffer)
        return AGPU_INVALID_OPERATION;

    currentVertexBinding->bind();
    currentIndexBuffer->bind();
    currentDrawBuffer->bind();
    
    device->glDrawElementsIndirect(mapPrimitiveMode(mode), mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset));
    
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::multiDrawElementsIndirect ( agpu_primitive_mode mode, agpu_size offset, agpu_size drawcount )
{
    if(!currentVertexBinding || !currentIndexBuffer || !currentDrawBuffer)
        return AGPU_INVALID_OPERATION;

    currentVertexBinding->bind();
    currentIndexBuffer->bind();
    currentDrawBuffer->bind();

    device->glMultiDrawElementsIndirect(mapPrimitiveMode(mode), mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset), drawcount, currentDrawBuffer->description.stride);
 
    return AGPU_OK;   
}

agpu_error AgpuGLImmediateContext::setUniform1i ( agpu_int location, agpu_size count, agpu_int* data )
{
    device->glUniform1iv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniform2i ( agpu_int location, agpu_size count, agpu_int* data )
{
    device->glUniform2iv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniform3i ( agpu_int location, agpu_size count, agpu_int* data )
{
    device->glUniform3iv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniform4i ( agpu_int location, agpu_size count, agpu_int* data )
{
    device->glUniform4iv(location, count, data);
    return AGPU_OK;
}

agpu_error AgpuGLImmediateContext::setUniform1f ( agpu_int location, agpu_size count, agpu_float* data )
{
    device->glUniform1fv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniform2f ( agpu_int location, agpu_size count, agpu_float* data )
{
    device->glUniform2fv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniform3f ( agpu_int location, agpu_size count, agpu_float* data )
{
    device->glUniform3fv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniform4f ( agpu_int location, agpu_size count, agpu_float* data )
{
    device->glUniform4fv(location, count, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniformMatrix2f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data )
{
    device->glUniformMatrix2fv(location, count, transpose, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniformMatrix3f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data )
{
    device->glUniformMatrix3fv(location, count, transpose, data);
    return AGPU_OK;    
}

agpu_error AgpuGLImmediateContext::setUniformMatrix4f ( agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data )
{
    device->glUniformMatrix4fv(location, count, transpose, data);
    return AGPU_OK;    
}

// Context dispatching
_agpu_context::_agpu_context(AgpuGLContext *impl)
    : impl(impl)
{
}

void _agpu_context::lostReferences()
{
    delete impl;
}

AGPU_EXPORT agpu_error agpuAddContextReference ( agpu_context* context )
{
    CHECK_POINTER(context);
    return context->retain();
}

AGPU_EXPORT agpu_error agpuReleaseContext ( agpu_context* context )
{
    CHECK_POINTER(context);
    return context->release();
}

AGPU_EXPORT agpu_error agpuSetClearColor ( agpu_context* context, agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
    CHECK_POINTER(context);
    return context->impl->setClearColor(r, g, b, a);
}

AGPU_EXPORT agpu_error agpuSetClearDepth ( agpu_context* context, agpu_float depth )
{
    CHECK_POINTER(context);
    return context->impl->setClearDepth(depth);
}

AGPU_EXPORT agpu_error agpuSetClearStencil ( agpu_context* context, agpu_int value )
{
    CHECK_POINTER(context);
    return context->impl->setClearStencil(value);
}

AGPU_EXPORT agpu_error agpuClear ( agpu_context* context, agpu_bitfield buffers )
{
    CHECK_POINTER(context);
    return context->impl->clear(buffers);
}

AGPU_EXPORT agpu_error agpuSetDepthFunction ( agpu_context* context, agpu_compare_function function )
{
    CHECK_POINTER(context);
    return context->impl->setDepthFunction(function);
}

AGPU_EXPORT agpu_error agpuSetAlphaFunction ( agpu_context* context, agpu_compare_function function, agpu_float reference )
{
    CHECK_POINTER(context);
    return context->impl->setAlphaFunction(function, reference);
}

AGPU_EXPORT agpu_error agpuMakeCurrent ( agpu_context* context )
{
    CHECK_POINTER(context);
    return context->impl->makeCurrent();
}

AGPU_EXPORT agpu_error agpuFlush ( agpu_context* context )
{
    CHECK_POINTER(context);
    return context->impl->flush();
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_context* context, agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(context);
    return context->impl->uploadBufferData(buffer, offset, size, data);
}

AGPU_EXPORT agpu_error agpuUseProgram ( agpu_context* context, agpu_program* program )
{
    CHECK_POINTER(context);
    CHECK_POINTER(program);
    return context->impl->useProgram(program);
}

AGPU_EXPORT agpu_error agpuUseVertexBinding ( agpu_context* context, agpu_vertex_binding* vertex_binding )
{
    CHECK_POINTER(context);
    CHECK_POINTER(vertex_binding);
    return context->impl->useVertexBinding(vertex_binding);
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer ( agpu_context* context, agpu_buffer* index_buffer )
{
    CHECK_POINTER(context);
    CHECK_POINTER(index_buffer);
    return context->impl->useIndexBuffer(index_buffer);
}

AGPU_EXPORT agpu_error agpuUseDrawBuffer ( agpu_context* context, agpu_buffer* draw_buffer )
{
    CHECK_POINTER(context);
    CHECK_POINTER(draw_buffer);
    return context->impl->useDrawBuffer(draw_buffer);
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect ( agpu_context* context, agpu_primitive_mode mode, agpu_size offset )
{
    CHECK_POINTER(context);
    return context->impl->drawElementsIndirect(mode, offset);
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect ( agpu_context* context, agpu_primitive_mode mode, agpu_size offset, agpu_size drawcount )
{
    CHECK_POINTER(context);
    return context->impl->multiDrawElementsIndirect(mode, offset, drawcount);
}

AGPU_EXPORT agpu_error agpuSetUniform1i ( agpu_context* context, agpu_int location, agpu_size count, agpu_int* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform1i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform2i ( agpu_context* context, agpu_int location, agpu_size count, agpu_int* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform2i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform3i ( agpu_context* context, agpu_int location, agpu_size count, agpu_int* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform3i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform4i ( agpu_context* context, agpu_int location, agpu_size count, agpu_int* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform4i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform1f ( agpu_context* context, agpu_int location, agpu_size count, agpu_float* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform1f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform2f ( agpu_context* context, agpu_int location, agpu_size count, agpu_float* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform2f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform3f ( agpu_context* context, agpu_int location, agpu_size count, agpu_float* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform3f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform4f ( agpu_context* context, agpu_int location, agpu_size count, agpu_float* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniform4f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix2f ( agpu_context* context, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data )
{
    CHECK_POINTER(context);
    CHECK_POINTER(data);
    return context->impl->setUniformMatrix2f(location, count, transpose, data);
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix3f ( agpu_context* context, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data )
{
    return context->impl->setUniformMatrix3f(location, count, transpose, data);
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix4f ( agpu_context* context, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data )
{
    return context->impl->setUniformMatrix4f(location, count, transpose, data);
}
