#ifndef AGPU_D3D12_COMMAND_ALLOCATOR_HPP_
#define AGPU_D3D12_COMMAND_ALLOCATOR_HPP_

#include "device.hpp"
#include "command_list.hpp"

namespace AgpuD3D12
{

class ADXCommandAllocator : public agpu::command_allocator
{
public:
    ADXCommandAllocator(const agpu::device_ref &device);
    ~ADXCommandAllocator();

    static agpu::command_allocator_ref create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &queue);

    agpu_error reset();

public:

    agpu::device_ref device;
    agpu_command_list_type type;
    ComPtr<ID3D12CommandAllocator> allocator;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_COMMAND_ALLOCATOR_HPP_
