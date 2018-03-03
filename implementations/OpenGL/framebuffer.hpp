#ifndef AGPU_GL_FRAMEBUFFER_HPP
#define AGPU_GL_FRAMEBUFFER_HPP

#include <vector>
#include "device.hpp"

struct _agpu_framebuffer : public Object<_agpu_framebuffer>
{
public:
    _agpu_framebuffer();

    void lostReferences();

    static agpu_framebuffer* create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorView, agpu_texture_view_description* depthStencilViews);

    agpu_error attachColorBuffer ( agpu_int index, agpu_texture_view_description* colorView);
    agpu_error attachDepthStencilBuffer (agpu_texture_view_description* depthStencilViews);

public:
    static const int MaxRenderTargetCount = 9;
    void bind(GLenum target = GL_FRAMEBUFFER);
    void updateAttachments(GLenum target);
    void attachTo(GLenum target, agpu_texture_view_description *view, GLenum attachmentPoint);

    agpu_device *device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;
    int renderTargetCount;
    agpu_texture_view_description colorBuffers[MaxRenderTargetCount];
    agpu_texture_view_description depthStencil;
    bool changed;
    GLuint handle;
};

#endif //AGPU_GL_FRAMEBUFFER_HPP
