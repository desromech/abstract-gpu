#ifndef AGPU_OPENGL_RENDERPASS_HPP
#define AGPU_OPENGL_RENDERPASS_HPP

#include "device.hpp"
#include <vector>

struct _agpu_renderpass : public Object<agpu_renderpass>
{
public:
    _agpu_renderpass(agpu_device *device);
    void lostReferences();

    static agpu_renderpass *create(agpu_device *device, agpu_renderpass_description *description);

    agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value);
    agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value);
    agpu_error setColorClearValueFrom ( agpu_uint attachment_index, agpu_color4f* value );

    void started();

    agpu_device *device;
    std::vector<agpu_renderpass_color_attachment_description> colorAttachments;
    agpu_renderpass_depth_stencil_description depthStencilAttachment;
    bool hasDepthStencil;
};

#endif // AGPU_OPENGL_RENDERPASS_HPP
