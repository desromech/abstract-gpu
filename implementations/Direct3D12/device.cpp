#include "device.hpp"

_agpu_device::_agpu_device()
{

}

void _agpu_device::lostReferences()
{

}

agpu_device *_agpu_device::open(agpu_device_open_info* openInfo)
{
    return nullptr;
}

agpu_error _agpu_device::swapBuffers()
{
    return AGPU_OK;
}

void _agpu_device::createDefaultCommandQueue()
{

}

// Exported C functions
AGPU_EXPORT agpu_error agpuAddDeviceReference(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->release();
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* device)
{
    if (!device)
        return nullptr;
    return device->defaultCommandQueue;
}

AGPU_EXPORT agpu_error agpuSwapBuffers(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->swapBuffers();
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    return nullptr;
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device)
{
    return nullptr;
}

AGPU_EXPORT agpu_shader* agpuCreateShader(agpu_device* device, agpu_shader_type type)
{
    return nullptr;
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* device)
{
    return nullptr;
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* device, agpu_pipeline_state* initial_pipeline_state)
{
    return nullptr;
}
