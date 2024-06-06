#ifdef _WIN32
#include "window_scraper.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#if WINAPI_PARTITION_DESKTOP
#include <thread>
#include <mutex>
#include <codecvt>

namespace AgpuCommon
{

class Win32WindowScraperHandle : public agpu::window_scraper_handle
{
public:
    Win32WindowScraperHandle(const agpu::device_ref &initialDevice, HWND initialHwnd)
        : device(initialDevice), hwnd(initialHwnd)
    {
    }

	virtual agpu_bool isValid() override
    {
        return ::IsWindow(hwnd);
    }

	virtual agpu_bool isVisible() override
    {
        return ::IsWindowVisible(hwnd);
    }

	virtual agpu_uint getWidth() override
    {
        RECT rect = {};
        GetClientRect(hwnd, &rect);
        return rect.right - rect.left;
    }

	virtual agpu_uint getHeight() override
    {
        RECT rect = {};
        GetClientRect(hwnd, &rect);
        return rect.bottom - rect.top;
    }
    
	virtual agpu::texture_ptr captureInTexture() override
    {
        if(!isValid())
            return nullptr;

        if(!isVisible())
            return lastCapturedTexture.disownedNewRef();

        RECT rect = {};
        GetClientRect(hwnd, &rect);
        if(IsRectEmpty(&rect))
            return lastCapturedTexture.disownedNewRef();

        HDC dc = GetDC(hwnd);
        if(!dc)
            return lastCapturedTexture.disownedNewRef();

        auto captureWidth = rect.right - rect.left;
        auto captureHeight = rect.bottom - rect.top;

        HDC memoryDc = CreateCompatibleDC(dc);

        HBITMAP memoryBitmapHandle = CreateCompatibleBitmap(dc, captureWidth, captureHeight);
        auto oldBitmap = SelectObject(memoryDc, memoryBitmapHandle);

        //StretchBlt(memoryDc, 0, 0, captureWidth, captureHeight, dc, rect.left, rect.top, captureWidth, captureHeight, SRCCOPY);
        BOOL bitBltSucceeded = BitBlt(memoryDc, 0, 0, captureWidth, captureHeight, dc, rect.left, rect.top, SRCCOPY);

        SelectObject(memoryDc, oldBitmap);

        // Get the bitmap
        agpu_int rowPitch = 0;
        agpu_int slicePitch = 0;

        if(bitBltSucceeded)
        {
            BITMAP memoryBitmap;
            GetObject(memoryBitmapHandle, sizeof(memoryBitmap), &memoryBitmap);

            BITMAPINFOHEADER infoHeader = {};
            infoHeader.biSize = sizeof(infoHeader);
            infoHeader.biWidth = memoryBitmap.bmWidth;
            infoHeader.biHeight = memoryBitmap.bmHeight;
            infoHeader.biPlanes = 1;
            infoHeader.biBitCount = 32;
            infoHeader.biCompression = BI_RGB;
            infoHeader.biSizeImage = 0;
            infoHeader.biXPelsPerMeter = 0;
            infoHeader.biYPelsPerMeter = 0;
            infoHeader.biClrUsed = 0;
            infoHeader.biClrImportant = 0;

            rowPitch = (infoHeader.biWidth*infoHeader.biBitCount + 31) / 32*4;
            slicePitch = rowPitch*infoHeader.biHeight;
            if(captureDataBuffer.size() < slicePitch)
                captureDataBuffer.resize(slicePitch);

            GetDIBits(memoryDc, memoryBitmapHandle, 0, memoryBitmap.bmHeight, captureDataBuffer.data(), reinterpret_cast<BITMAPINFO*>(&infoHeader), DIB_RGB_COLORS);
        }

        DeleteDC(memoryDc);
        DeleteObject(memoryBitmapHandle);
        ReleaseDC(hwnd, dc);

        if(!bitBltSucceeded)
            return lastCapturedTexture.disownedNewRef();
        
        if(!lastCapturedTexture || captureWidth != lastCapturedTextureDescription.width || captureHeight != lastCapturedTextureDescription.height)
        {
            agpu_texture_description description = {};
            description.type = AGPU_TEXTURE_2D;
            description.width = captureWidth;
            description.height = captureHeight;
            description.depth = 1;
            description.layers = 1;
            description.miplevels = 1;
            description.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB;
            description.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_COPY_DESTINATION | AGPU_TEXTURE_USAGE_SAMPLED);
            description.main_usage_mode = AGPU_TEXTURE_USAGE_SAMPLED;
            description.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
            description.sample_count = 1;
            description.sample_quality = 0;

            lastCapturedTexture = agpu::texture_ref(device->createTexture(&description));
            lastCapturedTextureDescription = description;
        }

        // Upload the bitmap into the texture.
        //printf("Upload texture %d %d\n", int(captureWidth), int(captureHeight));
        lastCapturedTexture->uploadTextureData(0, 0, rowPitch, slicePitch, captureDataBuffer.data());

        return lastCapturedTexture.disownedNewRef();
    }

    agpu::device_ref device;
    agpu::texture_ref lastCapturedTexture;
    agpu_texture_description lastCapturedTextureDescription;

    HWND hwnd;
    std::vector<uint8_t> captureDataBuffer;
};

class Win32WindowScraper : public WindowScraper
{
public:
    Win32WindowScraper(const agpu::device_ref &initialDevice)
        : WindowScraper(initialDevice)
    {
    }

	virtual agpu_uint enumerateWindows() override
    {
        enumeratedWindows.clear();
        enumeratedWindows.push_back(GetDesktopWindow());
        enumeratedWindowsTitle.push_back("[Desktop]");
        EnumWindows(onEnumWindowCallback, reinterpret_cast<LPARAM> (this));

        return agpu_uint(enumeratedWindows.size());
    }

	virtual agpu_cstring getWindowTitle(agpu_uint index) override
    {
        if(index < enumeratedWindowsTitle.size())
            return enumeratedWindowsTitle[index].c_str();
        return "";
    }

	virtual agpu::window_scraper_handle_ptr createWindowHandle(agpu_uint index) override
    {
        if(index < enumeratedWindows.size())
            return agpu::makeObject<Win32WindowScraperHandle> (device, enumeratedWindows[index]).disown();

        return nullptr;
    }

protected:
    static BOOL CALLBACK onEnumWindowCallback(HWND hwnd, LPARAM self)
    {
        if(!IsWindowVisible(hwnd))
            return TRUE;

        auto textLength = GetWindowTextLengthW(hwnd);
        std::vector<wchar_t> text;
        text.resize(textLength + 1);
        GetWindowTextW(hwnd, text.data(), textLength + 1);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        std::string utf8Name = convert.to_bytes(std::wstring(text.begin(), text.begin() + textLength));

        auto castedSelf = reinterpret_cast<Win32WindowScraper*> (self);
        castedSelf->enumeratedWindows.push_back(hwnd);
        castedSelf->enumeratedWindowsTitle.push_back(utf8Name);
        return TRUE;
    }

    std::vector<HWND> enumeratedWindows;
    std::vector<std::string> enumeratedWindowsTitle;
};

agpu::window_scraper_ref createWindowScraper(const agpu::device_ref &device)
{
	return agpu::makeObject<Win32WindowScraper> (device);
}

} // End of namespace AgpuCommon

#else
namespace AgpuCommon
{
agpu::window_scraper_ref createWindowScraper(const agpu::device_ref &device)
{
	return agpu::window_scraper_ref();
}
} // End of namespace AgpuCommon

#endif //#ifdef WINAPI_PARTITION_DESKTOP
#endif //_WIN32
