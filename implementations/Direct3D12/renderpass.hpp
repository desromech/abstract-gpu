#ifndef AGPU_D3D12_RENDERPASS_HPP
#define AGPU_D3D12_RENDERPASS_HPP

#include "device.hpp"
#include <vector>

namespace AgpuD3D12
{

class ADXRenderPass : public agpu::renderpass
{
public:
    ADXRenderPass(const agpu::device_ref &cdevice);
    ~ADXRenderPass();

    static agpu::renderpass_ref create(const agpu::device_ref &device, agpu_renderpass_description *description);

    virtual agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value) override;
    virtual agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value) override;
	virtual agpu_error setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f* value) override;

	virtual agpu_error getColorAttachmentFormats(agpu_uint* color_attachment_count, agpu_texture_format* formats) override;
	virtual agpu_texture_format getDepthStencilAttachmentFormat() override;
	virtual agpu_uint getSampleCount() override;
	virtual agpu_uint getSampleQuality() override;

    agpu::device_ref device;
    std::vector<agpu_renderpass_color_attachment_description> colorAttachments;
    agpu_renderpass_depth_stencil_description depthStencilAttachment;
    bool hasDepth;
    bool hasStencil;
	agpu_uint sampleCount;
	agpu_uint sampleQuality;
};

} // End of namespace AgpuD3D12

#endif // #ifndef AGPU_D3D12_RENDERPASS_HPP
