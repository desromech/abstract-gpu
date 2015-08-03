#ifndef _AGPU_DEVICE_HPP_
#define _AGPU_DEVICE_HPP_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d12.h>
#include "object.hpp"


/**
* Agpu D3D12 device
*/
struct _agpu_device : public Object<_agpu_device>
{
public:
    _agpu_device();

    void lostReferences();

    static agpu_device *open(agpu_device_open_info* openInfo);

    agpu_error swapBuffers();

    void createDefaultCommandQueue();

    HWND window;
    HDC hDC;

    agpu_command_queue *defaultCommandQueue;

public:

};

#endif //_AGPU_DEVICE_HPP_

