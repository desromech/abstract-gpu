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

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref &depthStencilView);

public:
    void bind(GLenum target = GL_FRAMEBUFFER);
    void updateAttachments(GLenum target);
    void attachTo(GLenum target, const agpu::texture_view_ref &view, GLenum attachmentPoint);

    agpu::device_ref device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;
    std::vector<agpu::texture_view_ref> colorBufferViews;
    std::vector<agpu::texture_ref> colorBufferTextures;
    agpu::texture_view_ref depthStencilView;
    agpu::texture_ref depthStencilTexture;

    bool changed;
    GLuint handle;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_FRAMEBUFFER_HPP
