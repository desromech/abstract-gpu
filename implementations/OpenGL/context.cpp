#include "context.hpp"
#include "buffer.hpp"
#include "program.hpp"

// Immediate context
AgpuGLImmediateContext::AgpuGLImmediateContext(agpu_device *device)
    : device(device)

{
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

