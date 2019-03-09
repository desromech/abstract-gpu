#ifndef AGPU_VULKAN_VR_SYSTEM_HPP
#define AGPU_VULKAN_VR_SYSTEM_HPP

#include "device.hpp"

class AgpuVkVRSystemSubmissionCommandBuffer
{
public:
    AgpuVkVRSystemSubmissionCommandBuffer();
    void lostReferences();

    bool initialize(agpu_device *device);
    agpu_error submitVREyeRenderTargets ( agpu_texture* left_eye, agpu_texture* right_eye );
    agpu_error doSubmissionOfEyeTextures ( agpu_texture* left_eye, agpu_texture* right_eye );
    agpu_error submitEyeTexture ( vr::Hmd_Eye eye, agpu_texture* texture);

    agpu_error beginCommandBuffer(VkCommandBuffer commandBuffer);
    agpu_error waitFence();

    agpu_device *device;

    VkFence fence;
    bool isFenceActive;

    VkCommandPool commandPool;
    VkCommandBuffer beforeSubmissionCommandList;
    VkCommandBuffer afterSubmissionCommandList;

    agpu_texture* leftEyeTexture;
    agpu_texture* rightEyeTexture;
};

struct _agpu_vr_system : public Object<_agpu_vr_system>
{
    static constexpr size_t FramebufferingSize = 3;

    _agpu_vr_system(agpu_device *device);
    void lostReferences();

    bool initialize();

    agpu_cstring getSystemName();
    agpu_pointer getSystemNativeHandle();
    agpu_error getRecommendedRenderTargetSize(agpu_size2d* size );
    agpu_error getEyeToHeadTransformInto(agpu_vr_eye eye, agpu_matrix4x4f* transform);
    agpu_error getProjectionMatrix ( agpu_vr_eye eye, agpu_float near, agpu_float far, agpu_matrix4x4f* projection_matrix );
    agpu_error getProjectionFrustumTangents(agpu_vr_eye eye, agpu_frustum_tangents* frustum);
    agpu_error submitVREyeRenderTargets ( agpu_texture* left_eye, agpu_texture* right_eye );

    agpu_error waitAndFetchVRPoses ();
    agpu_size getValidTrackedDevicePoseCount ();
    agpu_error getValidTrackedDevicePoseInto ( agpu_size index, agpu_vr_tracked_device_pose* dest );
    agpu_size getValidRenderTrackedDevicePoseCount ();
    agpu_error getValidRenderTrackedDevicePoseInto ( agpu_size index, agpu_vr_tracked_device_pose* dest );

    agpu_bool pollEvent ( agpu_vr_event* event );

    agpu_device *device;

private:
    agpu_vr_tracked_device_pose convertTrackedDevicePose(agpu_uint deviceId, const vr::TrackedDevicePose_t &devicePose);

    std::vector<agpu_vr_tracked_device_pose> validTrackedDevicePoses;
    std::vector<agpu_vr_tracked_device_pose> validRenderTrackedDevicePoses;
    vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];
    vr::TrackedDevicePose_t renderTrackedDevicesPose[vr::k_unMaxTrackedDeviceCount];

    std::mutex submissionMutex;

    AgpuVkVRSystemSubmissionCommandBuffer submissionCommandBuffers[FramebufferingSize];
    size_t submissionCommandBufferIndex;

};

#endif //AGPU_VULKAN_VR_SYSTEM_HPP
