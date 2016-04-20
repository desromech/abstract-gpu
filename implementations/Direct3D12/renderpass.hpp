#ifndef AGPU_D3D12_RENDERPASS_HPP
#define AGPU_D3D12_RENDERPASS_HPP

#include "device.hpp"
#include <vector>

struct _agpu_renderpass : public Object<agpu_renderpass>
{
public:
    _agpu_renderpass(agpu_device *device);
    void lostReferences();

    static agpu_renderpass *create(agpu_device *device, agpu_renderpass_description *description);

    agpu_device *device;
    std::vector<agpu_renderpass_color_attachment_description> colorAttachments;
    agpu_renderpass_depth_stencil_description depthStencilAttachment;
    bool hasDepth;
    bool hasStencil;
};

#endif // #ifndef AGPU_D3D12_RENDERPASS_HPP
