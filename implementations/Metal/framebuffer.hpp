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

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView);
    static agpu::framebuffer_ref createForSwapChain(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_texture_view_description* depthStencilView);

    void releaseDrawable();
    void setDrawable(id<MTLDrawable> drawable, id<MTLTexture> drawableTexture);
    id<MTLTexture> getColorTexture(agpu_uint index);

    agpu::device_ref device;
    agpu_uint width;
    agpu_uint height;
    std::vector<agpu::texture_ref> colorBuffers;
    std::vector<agpu_texture_view_description> colorBufferDescriptions;
    
    agpu::texture_ref depthStencilBuffer;
    agpu_texture_view_description depthStencilBufferDescription;
    agpu_bool ownedBySwapChain;
    id<MTLDrawable> drawable;
    id<MTLTexture> drawableTexture;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_FRAMEBUFFER_HPP
