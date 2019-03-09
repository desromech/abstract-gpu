#include "AGPU/agpu.h"

AGPU_EXPORT agpu_vr_system* agpuGetVRSystem ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuAddVRSystemReference ( agpu_vr_system* vr_system )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseVRSystem ( agpu_vr_system* vr_system )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_cstring agpuGetVRSystemName ( agpu_vr_system* vr_system )
{
    return "Dummy";
}

AGPU_EXPORT agpu_pointer agpuGetVRSystemNativeHandle ( agpu_vr_system* vr_system )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuGetVRRecommendedRenderTargetSize ( agpu_vr_system* vr_system, agpu_size2d* size )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetVREyeToHeadTransformInto ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_matrix4x4f* transform )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetVRProjectionMatrix ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetVRProjectionFrustumTangents ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_frustum_tangents* frustum )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSubmitVREyeRenderTargets ( agpu_vr_system* vr_system, agpu_texture* left_eye, agpu_texture* right_eye )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuWaitAndFetchVRPoses ( agpu_vr_system* vr_system )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_size agpuGetValidVRTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
    return 0;
}

AGPU_EXPORT agpu_error agpuGetValidVRTrackedDevicePoseInto ( agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_size agpuGetValidVRRenderTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
    return 0;
}

AGPU_EXPORT agpu_error agpuGetValidVRRenderTrackedDevicePoseInto ( agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_bool agpuPollVREvent ( agpu_vr_system* vr_system, agpu_vr_event* event )
{
    return false;
}
