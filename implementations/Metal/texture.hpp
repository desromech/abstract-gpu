#ifndef AGPU_METAL_TEXTURE_HPP
#define AGPU_METAL_TEXTURE_HPP

#include "device.hpp"

struct _agpu_texture : public Object<_agpu_texture>
{
public:
    _agpu_texture(agpu_device *device);
    void lostReferences();

    static agpu_texture *create(agpu_device *device, agpu_texture_description* description);

    agpu_error getDescription ( agpu_texture_description* description );
    agpu_pointer mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags );
    agpu_error unmapLevel (  );
    agpu_error readData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer );
    agpu_error uploadData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data );
    agpu_error discardUploadBuffer (  );
    agpu_error discardReadbackBuffer (  );
    agpu_error getFullViewDescription ( agpu_texture_view_description* result );

    agpu_device *device;
    agpu_texture_description description;

    id<MTLTexture> handle;

    MTLRegion getLevelRegion(int level)
    {
        MTLRegion region;
        memset(&region, 0, sizeof(region));

        region.size.width = description.width >> level;
        if (region.size.width == 0)
            region.size.width = 1;

        region.size.height = description.height >> level;
        if (description.type == AGPU_TEXTURE_1D || region.size.height == 0)
            region.size.height = 1;

        region.size.depth = description.depthOrArraySize >> level;
        if (description.type != AGPU_TEXTURE_3D || region.size.depth == 0)
            region.size.depth = 1;
        return region;
    }
};

#endif //AGPU_METAL_TEXTURE_HPP
