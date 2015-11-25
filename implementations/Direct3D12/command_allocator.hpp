#ifndef AGPU_D3D12_COMMAND_ALLOCATOR_HPP_
#define AGPU_D3D12_COMMAND_ALLOCATOR_HPP_

#include "device.hpp"
#include "command_list.hpp"

struct _agpu_command_allocator : public Object<_agpu_command_allocator>
{
public:
    _agpu_command_allocator();

    void lostReferences();

    static _agpu_command_allocator *create(agpu_device *device, agpu_command_list_type type);

    agpu_error reset();

public:

    agpu_device *device;
    agpu_command_list_type type;
    ComPtr<ID3D12CommandAllocator> allocator;
};

#endif //AGPU_D3D12_COMMAND_ALLOCATOR_HPP_
