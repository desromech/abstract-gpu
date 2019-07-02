#ifndef AGPU_OPENGL_SAMPLER_HPP
#define AGPU_OPENGL_SAMPLER_HPP

#include "device.hpp"

namespace AgpuGL
{

class GLSampler : public agpu::sampler
{
public:
    GLSampler(const agpu::device_ref &device);
    ~GLSampler();

    static agpu::sampler_ref create(const agpu::device_ref &device, agpu_sampler_description *description);

    agpu::device_ref device;
    agpu_sampler_description description;
    GLuint handle;
};

} // End of namespace AgpuGL

#endif //AGPU_OPENGL_SAMPLER_HPP
