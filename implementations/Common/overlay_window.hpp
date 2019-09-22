#ifndef _AGPU_COMMON_OVERLAY_WINDOW_HPP
#define _AGPU_COMMON_OVERLAY_WINDOW_HPP

#include <memory>
#include <string>
#include "AGPU/agpu.h"

namespace AgpuCommon
{

/**
 * I am an interface to an OS specific window handle that is used for overlaying
 * the content of a swap chain on top of an existing window, instead of replacing the full content of the window.
 */
class OverlaySwapChainWindow
{
public:
    OverlaySwapChainWindow(uint32_t cwidth, uint32_t cheight, int32_t cx, int32_t cy)
        : width(cwidth), height(cheight), x(cx), y(cy) {}
    virtual ~OverlaySwapChainWindow() {}

    virtual void *getDisplayHandle() = 0;
    virtual void *getOwnerWindowHandle() = 0;
    virtual void *getWindowHandle() = 0;

    uint32_t width, height;
    int32_t x, y;
};

typedef std::shared_ptr<OverlaySwapChainWindow> OverlaySwapChainWindowPtr;
OverlaySwapChainWindowPtr createOverlaySwapChainWindow(agpu_swap_chain_create_info *createInfo);

} // End of namespace AgpuCommon

#endif //_AGPU_COMMON_OVERLAY_WINDOW_HPP
