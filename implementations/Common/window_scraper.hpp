#ifndef AGPU_WINDOW_SCRAPER_HPP
#define AGPU_WINDOW_SCRAPER_HPP

#include <AGPU/agpu_impl.hpp>
#include <vector>
#include <string>

namespace AgpuCommon
{

class WindowScraper : public agpu::window_scraper
{
public:
    WindowScraper(const agpu::device_ref &initialDevice);

protected:
    agpu::device_ref device;
};

agpu::window_scraper_ref createWindowScraper(const agpu::device_ref &device);

} // End of namespace AgpuCommon

#endif //AGPU_WINDOW_SCRAPER_HPP