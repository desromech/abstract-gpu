#ifndef AGPU_METAL_FRAMEBUFFER_HPP
#define AGPU_METAL_FRAMEBUFFER_HPP

#include "device.hpp"
#include <vector>

namespace AgpuMetal
{
    
class AMtlFramebuffer: public agpu::framebuffer
{
public:
    AMtlFramebuffer(const agpu::device_ref &device);
    ~AMtlFramebuffer();

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref *colorViews, const agpu::texture_view_ref &depthStencilView);
    static agpu::framebuffer_ref createForSwapChain(const agpu::device_ref &device, agpu_uint width, agpu_uint height, const agpu::texture_view_ref &depthStencilView);

    virtual agpu_uint getWidth() override;
    virtual agpu_uint getHeight() override;
    
    void releaseDrawable();
    void setDrawable(id<MTLDrawable> drawable, id<MTLTexture> drawableTexture);
    id<MTLDrawable> borrowDrawable();
    id<MTLTexture> getColorTexture(agpu_uint index);

    agpu::device_ref device;
    agpu_uint width = 0;
    agpu_uint height = 0;
    std::vector<agpu::texture_ref> colorBuffers;
    std::vector<agpu::texture_view_ref> colorBufferViews;
    
    agpu::texture_ref depthStencilBuffer;
    agpu::texture_view_ref depthStencilBufferView;
    agpu_bool ownedBySwapChain;
    id<MTLDrawable> drawable;
    id<MTLTexture> drawableTexture;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_FRAMEBUFFER_HPP
