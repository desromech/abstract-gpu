#include "command_allocator.hpp"

namespace AgpuD3D12
{

ADXCommandAllocator::ADXCommandAllocator(const agpu::device_ref &cdevice)
    : device(cdevice)
{
}

ADXCommandAllocator::~ADXCommandAllocator()
{
}

agpu::command_allocator_ref ADXCommandAllocator::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &queue)
{
    // Create the command allocator.
    ComPtr<ID3D12CommandAllocator> allocator;
    if (FAILED(deviceForDX->d3dDevice->CreateCommandAllocator(mapCommandListType(type), IID_PPV_ARGS(&allocator))))
        return agpu::command_allocator_ref();

    auto res = agpu::makeObject<ADXCommandAllocator> (device);
    res.as<ADXCommandAllocator>()->allocator = allocator;
    return res;
}

agpu_error ADXCommandAllocator::reset()
{
    ERROR_IF_FAILED(allocator->Reset());
    return AGPU_OK;
}

} // End of namespace AgpuD3D12
