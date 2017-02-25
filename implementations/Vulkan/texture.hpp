#ifndef AGPU_TEXTURE_HPP
#define AGPU_TEXTURE_HPP

#include "device.hpp"

struct _agpu_texture : public Object<_agpu_texture>
{
    _agpu_texture(agpu_device *device);
    void lostReferences();

    static agpu_texture *create(agpu_device *device, agpu_texture_description *description);
    static agpu_texture *createFromImage(agpu_device *device, agpu_texture_description *description, VkImage image);
    static VkImageView createImageView(agpu_device *device, agpu_texture_view_description *viewDescription);

    agpu_error getFullViewDescription(agpu_texture_view_description *viewDescription);
    agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags);
    agpu_error unmapLevel();
    agpu_error readData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer);
    agpu_error uploadData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);
    agpu_error discardUploadBuffer();
    agpu_error discardReadbackBuffer();

    agpu_device *device;
    agpu_texture_description description;
    VkImage image;
    VkImageLayout initialLayout;
    VkAccessFlagBits initialLayoutAccessBits;
    VkDeviceMemory memory;
    bool owned;
    agpu_buffer *uploadBuffer;
    agpu_buffer *readbackBuffer;

    VkExtent3D getLevelExtent(int level);
    void computeBufferImageTransferLayout(int level, VkSubresourceLayout *layout, VkBufferImageCopy *copy);
};

#endif //AGPU_TEXTURE_HPP
