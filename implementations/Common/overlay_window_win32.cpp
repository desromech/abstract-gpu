#ifdef _WIN32
#include "overlay_window.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <thread>
#include <mutex>

namespace AgpuCommon
{

class OverlaySwapChainWin32Window : public OverlaySwapChainWindow
{
public:
    OverlaySwapChainWin32Window(uint32_t cwidth, uint32_t cheight, int32_t cx, int32_t cy, HWND cparentWindowHandle, HWND cwindowHandle);
    ~OverlaySwapChainWin32Window();

	virtual void* getDisplayHandle() override;
	virtual void* getOwnerWindowHandle() override;
	virtual void* getWindowHandle() override;

	virtual agpu_error setSize(uint32_t newWidth, uint32_t newHeight) override;
	virtual agpu_error setPosition(int32_t newX, int32_t newY) override;
    virtual agpu_error setPositionAndSize(int32_t newX, int32_t newY, uint32_t newWidth, uint32_t newHeight) override;

    HWND parentWindowHandle;
    HWND windowHandle;
};

static std::once_flag registeredWindowClassFlag;

static LRESULT CALLBACK swapChainOverlayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT paintStruct;
			auto hdc = BeginPaint(hWnd, &paintStruct);
			EndPaint(hWnd, &paintStruct);
		}
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

OverlaySwapChainWindowPtr createOverlaySwapChainWin32Window(agpu_swap_chain_create_info *createInfo)
{
	std::call_once(registeredWindowClassFlag, [] {
		WNDCLASSEXW windowClass = {};
		windowClass.cbSize = sizeof(windowClass);
		windowClass.lpszClassName = L"AGPUSwapChainOverlayWindowClass";
		windowClass.lpfnWndProc = swapChainOverlayWindowProc;
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClassExW(&windowClass);
	});

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HWND overlayHwnd = CreateWindowExW(WS_EX_TRANSPARENT, L"AGPUSwapChainOverlayWindowClass", L"AGPUOverlay", WS_CHILD | WS_VISIBLE | WS_DISABLED , createInfo->x, createInfo->y, createInfo->width, createInfo->height, (HWND)createInfo->window, NULL, hInstance, NULL);
	if (!overlayHwnd)
		return nullptr;

	// Disable the window to propagate the input messages to the parent.
	EnableWindow(overlayHwnd, FALSE);

    return std::make_shared<OverlaySwapChainWin32Window> (createInfo->x, createInfo->y, createInfo->width, createInfo->height, (HWND)createInfo->window, overlayHwnd);
}

OverlaySwapChainWin32Window::OverlaySwapChainWin32Window(uint32_t cwidth, uint32_t cheight, int32_t cx, int32_t cy, HWND cparentWindowHandle, HWND cwindowHandle)
    : OverlaySwapChainWindow(cwidth, cheight, cx, cy),
      parentWindowHandle(cparentWindowHandle),
      windowHandle(cwindowHandle)
{
}

OverlaySwapChainWin32Window::~OverlaySwapChainWin32Window()
{
	DestroyWindow(windowHandle);
}

void* OverlaySwapChainWin32Window::getDisplayHandle()
{
	return nullptr;
}

void* OverlaySwapChainWin32Window::getOwnerWindowHandle()
{
	return parentWindowHandle;
}

void* OverlaySwapChainWin32Window::getWindowHandle()
{
	return windowHandle;
}

agpu_error OverlaySwapChainWin32Window::setSize(uint32_t newWidth, uint32_t newHeight)
{
	SetWindowPos(windowHandle, HWND_TOP, x, y, newWidth, newHeight, SWP_NOMOVE |SWP_NOZORDER);
	width = newWidth;
	height = newHeight;
	return AGPU_OK;
}

agpu_error OverlaySwapChainWin32Window::setPosition(int32_t newX, int32_t newY)
{
	SetWindowPos(windowHandle, HWND_TOP, newX, newY, width, height, SWP_NOSIZE|SWP_NOZORDER);
	return AGPU_OK;
}

agpu_error OverlaySwapChainWin32Window::setPositionAndSize(int32_t newX, int32_t newY, uint32_t newWidth, uint32_t newHeight)
{
    SetWindowPos(windowHandle, HWND_TOP, newX, newY, newWidth, newHeight, SWP_NOZORDER);
    x = newX;
	y = newY;
	width = newWidth;
	height = newHeight;
    return AGPU_OK;
}

} // End of namespace AgpuCommon
#endif //_WIN32
