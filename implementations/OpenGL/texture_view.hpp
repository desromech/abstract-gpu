#ifndef AGPU_TEXTURE_VIEW_HPP
#define AGPU_TEXTURE_VIEW_HPP

#include "device.hpp"

namespace AgpuGL
{

class GLAbstractTextureView : public agpu::texture_view
{
public:
    GLAbstractTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description);
    ~GLAbstractTextureView();

	virtual agpu::texture_ptr getTexture() override;
    virtual void activateInSampledSlot(int slotIndex) = 0;
    virtual void attachToFramebuffer(GLenum target, GLenum attachmentPoint) = 0;

    agpu::device_ref device;
    agpu::texture_weakref texture;
    agpu_texture_view_description description;
};

class GLFullTextureView : public GLAbstractTextureView
{
public:
    GLFullTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description);
    ~GLFullTextureView();

    virtual void activateInSampledSlot(int slotIndex) override;
    virtual void attachToFramebuffer(GLenum target, GLenum attachmentPoint) override;

};

} // End of namespace AgpuGL

#endif //AGPU_TEXTURE_VIEW_HPP
