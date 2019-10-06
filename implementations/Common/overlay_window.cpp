#include "overlay_window.hpp"

namespace AgpuCommon
{

#ifdef _WIN32
OverlaySwapChainWindowPtr createOverlaySwapChainWin32Window(agpu_swap_chain_create_info *createInfo);
#endif

OverlaySwapChainWindowPtr createOverlaySwapChainWindow(agpu_swap_chain_create_info *createInfo)
{
    std::string windowSystemName = createInfo->window_system_name ? createInfo->window_system_name : "";
#ifdef _WIN32
    if(windowSystemName == "win32" || windowSystemName.empty())
        return createOverlaySwapChainWin32Window(createInfo);
#endif

    return OverlaySwapChainWindowPtr();
}

} // End of namespace AgpuCommon
