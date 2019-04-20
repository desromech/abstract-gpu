#ifndef AGPU_OPENGL_RENDERPASS_HPP
#define AGPU_OPENGL_RENDERPASS_HPP

#include "device.hpp"
#include <vector>

namespace AgpuGL
{

struct GLRenderPass : public agpu::renderpass
{
public:
    GLRenderPass(const agpu::device_ref &device);
    ~GLRenderPass();

    static agpu::renderpass_ref create(const agpu::device_ref &device, agpu_renderpass_description *description);

    agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value);
    agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value);
    agpu_error setColorClearValueFrom ( agpu_uint attachment_index, agpu_color4f* value );

    void started();

    agpu::device_ref device;
    std::vector<agpu_renderpass_color_attachment_description> colorAttachments;
    agpu_renderpass_depth_stencil_description depthStencilAttachment;
    bool hasDepthStencil;
};

} // End of namespace AgpuGL

#endif // AGPU_OPENGL_RENDERPASS_HPP
