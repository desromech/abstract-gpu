#include "vr_system.hpp"

inline vr::Hmd_Eye mapVREye(agpu_vr_eye eye)
{
    switch(eye)
    {
    case AGPU_VR_EYE_LEFT: return vr::Eye_Left;
    case AGPU_VR_EYE_RIGHT: return vr::Eye_Right;
    default: abort();
    }
}

inline agpu_matrix4x4f convertOVRMatrix(vr::HmdMatrix34_t matrix)
{
    agpu_matrix4x4f result = {
        {matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f}, // C0
        {matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f}, // C1
        {matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f}, // C2
        {matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f}  // C3
    };

    return result;
}

inline agpu_matrix4x4f convertOVRMatrix(vr::HmdMatrix44_t matrix)
{
    agpu_matrix4x4f result = {
        {matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0]}, // C0
        {matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1]}, // C1
        {matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2]}, // C2
        {matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]}  // C3
    };

    return result;
}
_agpu_vr_system::_agpu_vr_system(agpu_device *cdevice)
    : device(cdevice)
{
}

void _agpu_vr_system::lostReferences()
{
}

agpu_cstring _agpu_vr_system::getSystemName()
{
    return "OpenVR";
}

agpu_pointer _agpu_vr_system::getSystemNativeHandle()
{
    return device->vrSystem;
}

agpu_error _agpu_vr_system::getRecommendedRenderTargetSize(agpu_size2d* size )
{
    CHECK_POINTER(size);

    uint32_t width, height;
    device->vrSystem->GetRecommendedRenderTargetSize(&width, &height);
    size->width = width;
    size->height = height;
    return AGPU_OK;
}

agpu_error _agpu_vr_system::getEyeToHeadTransformInto(agpu_vr_eye eye, agpu_matrix4x4f* transform)
{
    CHECK_POINTER(transform);
    *transform = convertOVRMatrix(device->vrSystem->GetEyeToHeadTransform(mapVREye(eye)));
    return AGPU_OK;
}


agpu_error _agpu_vr_system::getProjectionMatrix ( agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix )
{
    CHECK_POINTER(projection_matrix);
    *projection_matrix = convertOVRMatrix(device->vrSystem->GetProjectionMatrix(mapVREye(eye), near_distance, far_distance));
    return AGPU_OK;
}

agpu_error _agpu_vr_system::getProjectionFrustumTangents(agpu_vr_eye eye, agpu_frustum_tangents* frustum)
{
    CHECK_POINTER(frustum);

    float left, right, top, bottom;
    device->vrSystem->GetProjectionRaw(mapVREye(eye), &left, &right, &top, &bottom);

    frustum->left = left;
    frustum->right = right;
    frustum->top = top;
    frustum->bottom = bottom;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_vr_system* agpuGetVRSystem ( agpu_device* device )
{
    if(!device)
        return nullptr;

    return device->vrSystemWrapper;
}

AGPU_EXPORT agpu_error agpuAddVRSystemReference ( agpu_vr_system* vr_system )
{
    CHECK_POINTER(vr_system);
    return vr_system->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVRSystem ( agpu_vr_system* vr_system )
{
    CHECK_POINTER(vr_system);
    return vr_system->release();
}

AGPU_EXPORT agpu_cstring agpuGetVRSystemName ( agpu_vr_system* vr_system )
{
    if(!vr_system)
        return "Dummy";
    return vr_system->getSystemName();
}

AGPU_EXPORT agpu_pointer agpuGetVRSystemNativeHandle ( agpu_vr_system* vr_system )
{
    if(!vr_system)
        return nullptr;
    return vr_system->getSystemNativeHandle();
}

AGPU_EXPORT agpu_error agpuGetVRRecommendedRenderTargetSize ( agpu_vr_system* vr_system, agpu_size2d* size )
{
    CHECK_POINTER(vr_system);
    return vr_system->getRecommendedRenderTargetSize(size);
}

AGPU_EXPORT agpu_error agpuGetVREyeToHeadTransformInto ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_matrix4x4f* transform )
{
    CHECK_POINTER(vr_system);
    return vr_system->getEyeToHeadTransformInto(eye, transform);
}

AGPU_EXPORT agpu_error agpuGetVRProjectionMatrix ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix )
{
    CHECK_POINTER(vr_system);
    return vr_system->getProjectionMatrix(eye, near_distance, far_distance, projection_matrix);
}

AGPU_EXPORT agpu_error agpuGetVRProjectionFrustumTangents ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_frustum_tangents* frustum )
{
    CHECK_POINTER(vr_system);
    return vr_system->getProjectionFrustumTangents(eye, frustum);
}
