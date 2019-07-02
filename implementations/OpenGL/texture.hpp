#ifndef AGPU_TEXTURE_HPP
#define AGPU_TEXTURE_HPP

#include "device.hpp"

namespace AgpuGL
{

struct BufferTextureTransferLayout
{
    static BufferTextureTransferLayout fromDescriptionAndLevel(const agpu_texture_description &description, int level);
    void setFromDescriptionAndLevel(const agpu_texture_description &description, int level);

    agpu_int pitch;
    agpu_int slicePitch;
    agpu_size width;
    agpu_size height;
    agpu_size depthOrArraySize;
    agpu_size size;

    agpu_size logicalWidth;
    agpu_size logicalHeight;
    agpu_size logicalDepthOrArraySize;
};

struct GLTexture : public agpu::texture
{
public:
    GLTexture();
    ~GLTexture();

    static agpu::texture_ref create(const agpu::device_ref &device, agpu_texture_description *description);

    virtual agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d *region) override;
    virtual agpu_error unmapLevel() override;

    virtual agpu_error getFullViewDescription(agpu_texture_view_description *viewDescription) override;
	virtual agpu::texture_view_ptr createView(agpu_texture_view_description* description) override;
	virtual agpu::texture_view_ptr getOrCreateFullView() override;

    virtual agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) override;
    virtual agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) override;
    virtual agpu_error uploadTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data) override;

    virtual agpu_error discardUploadBuffer() override;
    virtual agpu_error discardReadbackBuffer() override;

    virtual agpu_error getDescription(agpu_texture_description* description) override;


public:
    agpu::device_ref device;
    agpu_texture_description description;
    GLuint handle;
    GLenum target;

    GLuint transferBuffer;
    agpu_mapping_access mappingAccess;
    agpu_int mappedLevel;
    agpu_uint mappedArrayIndex;
    agpu_pointer mappedPointer;
    bool isCompressed;
    agpu::texture_view_ref fullTextureView;

private:
    static void allocateTexture(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture1D(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture2D(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture3D(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTextureCube(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTextureBuffer(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description);

    void createTransferBuffer(GLenum target);
    void performTransferToCpu(int level);
    void performTransferToGpu(int level, int arrayIndex);
};

} // End of namespace AgpuGL

#endif //AGPU_TEXTURE_HPP
