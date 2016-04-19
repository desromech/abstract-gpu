#ifndef AGPU_BUFFER_HPP
#define AGPU_BUFFER_HPP

#include "device.hpp"

struct _agpu_buffer : public Object<_agpu_buffer>
{
public:
    _agpu_buffer(agpu_device *device);
    void lostReferences();

    static agpu_buffer* create ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );

    agpu_pointer map ( agpu_mapping_access flags );
    agpu_error unmap (  );
    agpu_error getDescription ( agpu_buffer_description* description );
    agpu_error uploadData ( agpu_size offset, agpu_size size, agpu_pointer data );
    agpu_error readData ( agpu_size offset, agpu_size size, agpu_pointer data );

    agpu_device *device;
    agpu_buffer_description description;
    id<MTLBuffer> handle;
};

#endif //AGPU_BUFFER_HPP
