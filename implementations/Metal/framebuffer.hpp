#ifndef AGPU_METAL_FRAMEBUFFER_HPP
#define AGPU_METAL_FRAMEBUFFER_HPP

#include "device.hpp"
#include <vector>

struct _agpu_framebuffer: public Object<agpu_framebuffer>
{
public:
    _agpu_framebuffer(agpu_device *device);
    void lostReferences();

    static agpu_framebuffer* create ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView );

    agpu_device *device;
    agpu_uint width;
    agpu_uint height;
    std::vector<agpu_texture* > colorBuffers;
    agpu_texture *depthStencilBuffer;
    MTLRenderPassDescriptor *renderPass;
};

#endif //AGPU_METAL_FRAMEBUFFER_HPP
