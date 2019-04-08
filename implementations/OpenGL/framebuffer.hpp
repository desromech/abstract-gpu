#ifndef AGPU_GL_FRAMEBUFFER_HPP
#define AGPU_GL_FRAMEBUFFER_HPP

#include <vector>
#include "device.hpp"

namespace AgpuGL
{

struct GLFramebuffer : public agpu::framebuffer
{
public:
    GLFramebuffer();
    ~GLFramebuffer();

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorView, agpu_texture_view_description* depthStencilViews);

    agpu_error attachColorBuffer ( agpu_int index, agpu_texture_view_description* colorView);
    agpu_error attachDepthStencilBuffer (agpu_texture_view_description* depthStencilViews);

public:
    static const int MaxRenderTargetCount = 9;
    void bind(GLenum target = GL_FRAMEBUFFER);
    void updateAttachments(GLenum target);
    void attachTo(GLenum target, agpu_texture_view_description *view, const agpu::texture_ref &texture, GLenum attachmentPoint);

    agpu::device_ref device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;
    int renderTargetCount;
    agpu_texture_view_description colorBuffers[MaxRenderTargetCount];
    agpu_texture_view_description depthStencil;
    agpu::texture_ref colorBufferTextures[MaxRenderTargetCount];
    agpu::texture_ref depthStencilTexture;

    bool changed;
    GLuint handle;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_FRAMEBUFFER_HPP
