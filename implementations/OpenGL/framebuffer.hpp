#ifndef AGPU_GL_FRAMEBUFFER_HPP
#define AGPU_GL_FRAMEBUFFER_HPP

#include <vector>
#include "device.hpp"

struct _agpu_framebuffer : public Object<_agpu_framebuffer>
{
public:
    _agpu_framebuffer();

    void lostReferences();

    static agpu_framebuffer* create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil);

public:
    static const int MaxRenderTargetCount = 9;
    void bind();

    agpu_device *device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;
    int renderTargetCount;
    agpu_texture *colorBuffers[MaxRenderTargetCount];
    agpu_texture *depthStencil;
};

#endif //AGPU_GL_FRAMEBUFFER_HPP
