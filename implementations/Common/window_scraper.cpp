#include "window_scraper.hpp"

namespace AgpuCommon
{

WindowScraper::WindowScraper(const agpu::device_ref &initialDevice)
    : device(initialDevice)
{
}

#ifndef _WIN32
agpu::window_scraper_ref createWindowScraper(const agpu::device_ref &device)
{
	return agpu::window_scraper_ref();
}
#endif

} // End of namespace AgpuCommon