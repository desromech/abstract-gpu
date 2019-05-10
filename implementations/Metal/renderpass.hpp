#ifndef AGPU_METAL_RENDERPASS_HPP
#define AGPU_METAL_RENDERPASS_HPP

#include "device.hpp"
#include <vector>

namespace AgpuMetal
{
    
class AMtlRenderPass : public agpu::renderpass
{
public:
    AMtlRenderPass(const agpu::device_ref &device);
    ~AMtlRenderPass();

    static agpu::renderpass_ref create(const agpu::device_ref &device, agpu_renderpass_description *description);

    MTLRenderPassDescriptor *createDescriptor(const agpu::framebuffer_ref &framebuffer);
    agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value);
    agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value);
    agpu_error setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f *value);

    agpu::device_ref device;
    bool hasDepthStencil;
    bool hasStencil;
    agpu_renderpass_depth_stencil_description depthStencil;
    std::vector<agpu_renderpass_color_attachment_description> colorAttachments;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_RENDERPASS_HPP
