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

    agpu_device *device;
    agpu_texture_description description;
    VkImage image;
    VkDeviceMemory memory;
    bool owned;

};

#endif //AGPU_TEXTURE_HPP