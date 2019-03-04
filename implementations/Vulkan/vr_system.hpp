#ifndef AGPU_VULKAN_VR_SYSTEM_HPP
#define AGPU_VULKAN_VR_SYSTEM_HPP

#include "device.hpp"

struct _agpu_vr_system : public Object<_agpu_vr_system>
{
    _agpu_vr_system(agpu_device *device);
    void lostReferences();

    agpu_cstring getSystemName();
    agpu_pointer getSystemNativeHandle();
    agpu_error getRecommendedRenderTargetSize(agpu_size2d* size );
    agpu_error getEyeToHeadTransformInto(agpu_vr_eye eye, agpu_matrix4x4f* transform);
    agpu_error getProjectionMatrix ( agpu_vr_eye eye, agpu_float near, agpu_float far, agpu_matrix4x4f* projection_matrix );
    agpu_error getProjectionFrustumTangents(agpu_vr_eye eye, agpu_frustum_tangents* frustum);

    agpu_device *device;
};

#endif //AGPU_VULKAN_FRAMEBUFFER_HPP
