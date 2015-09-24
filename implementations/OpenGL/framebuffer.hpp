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
    static agpu_framebuffer* createMain(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil);

    agpu_error createImplicitDepthStencil(agpu_uint depthSize, agpu_uint stencilSize);

public:
    void bind();

    agpu_device *device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;
    int renderTargetCount;
};

#endif //AGPU_GL_FRAMEBUFFER_HPP
