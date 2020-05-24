#ifndef AGPU_VULKAN_VR_SYSTEM_HPP
#define AGPU_VULKAN_VR_SYSTEM_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AgpuVkVRSystemSubmissionCommandBuffer
{
public:
    AgpuVkVRSystemSubmissionCommandBuffer();
    ~AgpuVkVRSystemSubmissionCommandBuffer();

    bool initialize(const agpu::device_ref &device);
    agpu_error submitVREyeRenderTargets(const agpu::texture_ref &left_eye, const agpu::texture_ref &right_eye);
    agpu_error doSubmissionOfEyeTextures(const agpu::texture_ref &left_eye, const agpu::texture_ref &right_eye);
    agpu_error submitEyeTexture(vr::Hmd_Eye eye, const agpu::texture_ref &texture);

    agpu_error beginCommandBuffer(VkCommandBuffer commandBuffer);
    agpu_error waitFence();

    AVkDeviceSharedContextPtr sharedContext;
    agpu::device_weakref weakDevice;

    VkFence fence;
    bool isFenceActive;

    VkCommandPool commandPool;
    VkCommandBuffer beforeSubmissionCommandList;
    VkCommandBuffer afterSubmissionCommandList;

    agpu::texture_ref leftEyeTexture;
    agpu::texture_ref rightEyeTexture;
};

class AVkVrSystem : public agpu::vr_system
{
public:
    static constexpr size_t FramebufferingSize = 3;

    AVkVrSystem(const agpu::device_ref &device);
    ~AVkVrSystem();

    bool initialize();

    virtual agpu_cstring getVRSystemName() override;
    virtual agpu_pointer getNativeHandle() override;
    virtual agpu_error getRecommendedRenderTargetSize(agpu_size2d* size ) override;
    virtual agpu_error getEyeToHeadTransform(agpu_vr_eye eye, agpu_matrix4x4f* transform) override;
    virtual agpu_error getProjectionMatrix( agpu_vr_eye eye, agpu_float near, agpu_float far, agpu_matrix4x4f* projection_matrix ) override;
    virtual agpu_error getProjectionFrustumTangents(agpu_vr_eye eye, agpu_frustum_tangents* frustum) override;
    virtual agpu_error submitEyeRenderTargets(const agpu::texture_ref & left_eye, const agpu::texture_ref & right_eye) override;

    virtual agpu_error waitAndFetchPoses() override;
    virtual agpu_size getMaxTrackedDevicePoseCount() override;
	virtual agpu_size getCurrentTrackedDevicePoseCount() override;
	virtual agpu_error getCurrentTrackedDevicePoseInto(agpu_size index, agpu_vr_tracked_device_pose* dest) override;
	virtual agpu_size getMaxRenderTrackedDevicePoseCount() override;
	virtual agpu_size getCurrentRenderTrackedDevicePoseCount() override;
	virtual agpu_error getCurrentRenderTrackedDevicePoseInto(agpu_size index, agpu_vr_tracked_device_pose* dest) override;

    virtual agpu_bool pollEvent ( agpu_vr_event* event ) override;

    agpu::device_weakref weakDevice;
    AVkDeviceSharedContextPtr sharedContext;

private:
    agpu_vr_tracked_device_pose convertTrackedDevicePose(agpu_uint deviceId, const vr::TrackedDevicePose_t &devicePose);

    std::vector<agpu_vr_tracked_device_pose> currentTrackedDevicePoses;
    std::vector<agpu_vr_tracked_device_pose> currentRenderTrackedDevicePoses;
    vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];
    vr::TrackedDevicePose_t renderTrackedDevicesPose[vr::k_unMaxTrackedDeviceCount];

    std::mutex submissionMutex;

    AgpuVkVRSystemSubmissionCommandBuffer submissionCommandBuffers[FramebufferingSize];
    size_t submissionCommandBufferIndex;

};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_VR_SYSTEM_HPP
