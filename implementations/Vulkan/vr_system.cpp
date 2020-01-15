#include "vr_system.hpp"
#include "command_queue.hpp"
#include "texture.hpp"
#include "texture_format.hpp"

namespace AgpuVulkan
{

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

inline agpu_vector3f convertOVRVector3(vr::HmdVector3_t vector)
{
    agpu_vector3f result = {vector.v[0], vector.v[1],vector.v[2]};
    return result;
}

inline agpu_vr_tracked_device_class mapTrackedDeviceClass(vr::TrackedDeviceClass clazz)
{
    switch(clazz)
    {
    case vr::TrackedDeviceClass_Invalid:
    default: return AGPU_VR_TRACKED_DEVICE_CLASS_INVALID;

    case vr::TrackedDeviceClass_HMD: return AGPU_VR_TRACKED_DEVICE_CLASS_HMD;
	case vr::TrackedDeviceClass_Controller: return AGPU_VR_TRACKED_DEVICE_CLASS_CONTROLLER;
	case vr::TrackedDeviceClass_GenericTracker: return AGPU_VR_TRACKED_DEVICE_CLASS_GENERIC_TRACKER;
	case vr::TrackedDeviceClass_TrackingReference: return AGPU_VR_TRACKED_DEVICE_CLASS_TRACKING_REFERENCE;
	case vr::TrackedDeviceClass_DisplayRedirect: return AGPU_VR_TRACKED_DEVICE_CLASS_DISPLAY_REDIRECT;
    }
}

AVkVrSystem::AVkVrSystem(const agpu::device_ref &cdevice)
    : weakDevice(cdevice), submissionCommandBufferIndex(0)
{
    currentTrackedDevicePoses.resize(vr::k_unMaxTrackedDeviceCount);
    currentRenderTrackedDevicePoses.resize(vr::k_unMaxTrackedDeviceCount);
}

AVkVrSystem::~AVkVrSystem()
{
}

bool AVkVrSystem::initialize()
{
    auto device = weakDevice.lock();
    for(auto &submissionCommandBuffer: submissionCommandBuffers)
    {
        if(!submissionCommandBuffer.initialize(device))
            return false;
    }

    return true;
}

agpu_cstring AVkVrSystem::getVRSystemName()
{
    return "OpenVR";
}

agpu_pointer AVkVrSystem::getNativeHandle()
{
    auto device = weakDevice.lock();
    return deviceForVk->vrSystem;
}

agpu_error AVkVrSystem::getRecommendedRenderTargetSize(agpu_size2d* size )
{
    CHECK_POINTER(size);

    uint32_t width, height;
    auto device = weakDevice.lock();
    deviceForVk->vrSystem->GetRecommendedRenderTargetSize(&width, &height);
    size->width = width;
    size->height = height;
    return AGPU_OK;
}

agpu_error AVkVrSystem::getEyeToHeadTransform(agpu_vr_eye eye, agpu_matrix4x4f* transform)
{
    CHECK_POINTER(transform);
    auto device = weakDevice.lock();
    *transform = convertOVRMatrix(deviceForVk->vrSystem->GetEyeToHeadTransform(mapVREye(eye)));
    return AGPU_OK;
}

agpu_error AVkVrSystem::getProjectionMatrix ( agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix )
{
    CHECK_POINTER(projection_matrix);
    auto device = weakDevice.lock();
    *projection_matrix = convertOVRMatrix(deviceForVk->vrSystem->GetProjectionMatrix(mapVREye(eye), near_distance, far_distance));
    return AGPU_OK;
}

agpu_error AVkVrSystem::getProjectionFrustumTangents(agpu_vr_eye eye, agpu_frustum_tangents* frustum)
{
    CHECK_POINTER(frustum);

    float left, right, top, bottom;
    auto device = weakDevice.lock();
    deviceForVk->vrSystem->GetProjectionRaw(mapVREye(eye), &left, &right, &top, &bottom);

    frustum->left = left;
    frustum->right = right;

    // For vulkan, invert top and bottom.
    frustum->top = bottom;
    frustum->bottom = top;
    return AGPU_OK;
}

agpu_error AVkVrSystem::submitEyeRenderTargets(const agpu::texture_ref & left_eye, const agpu::texture_ref & right_eye)
{
    std::unique_lock<std::mutex> l(submissionMutex);

    auto error = submissionCommandBuffers[submissionCommandBufferIndex].submitVREyeRenderTargets (left_eye, right_eye);
    if(error)
        return error;

    submissionCommandBufferIndex = (submissionCommandBufferIndex + 1) % FramebufferingSize;
    return AGPU_OK;
}

agpu_vr_tracked_device_pose AVkVrSystem::convertTrackedDevicePose(agpu_uint deviceId, const vr::TrackedDevicePose_t &devicePose)
{
    agpu_vr_tracked_device_pose convertedPose = {};
    convertedPose.is_valid = devicePose.bPoseIsValid ? 1 : 0;
    if(!convertedPose.is_valid) return convertedPose;

    auto device = weakDevice.lock();
    convertedPose.device_id = deviceId;
    convertedPose.device_class = mapTrackedDeviceClass(deviceForVk->vrSystem->GetTrackedDeviceClass(deviceId));
    convertedPose.device_role = AGPU_VR_TRACKED_DEVICE_ROLE_INVALID;

    convertedPose.device_to_absolute_tracking = convertOVRMatrix(devicePose.mDeviceToAbsoluteTracking);
    convertedPose.velocity = convertOVRVector3(devicePose.vVelocity);
	convertedPose.angular_velocity = convertOVRVector3(devicePose.vAngularVelocity);

    return convertedPose;
}


agpu_error AVkVrSystem::waitAndFetchPoses()
{
    vr::VRCompositor()->WaitGetPoses(trackedDevicesPose, vr::k_unMaxTrackedDeviceCount, renderTrackedDevicesPose, vr::k_unMaxTrackedDeviceCount );

    for(size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
        currentTrackedDevicePoses[i]  = convertTrackedDevicePose(i, trackedDevicesPose[i]);

    for(size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
        currentRenderTrackedDevicePoses[i]  = convertTrackedDevicePose(i, renderTrackedDevicesPose[i]);

    return AGPU_OK;
}

agpu_size AVkVrSystem::getMaxTrackedDevicePoseCount()
{
    return vr::k_unMaxTrackedDeviceCount;
}

agpu_size AVkVrSystem::getMaxRenderTrackedDevicePoseCount()
{
    return vr::k_unMaxTrackedDeviceCount;
}

agpu_size AVkVrSystem::getCurrentTrackedDevicePoseCount()
{
    return currentTrackedDevicePoses.size();
}

agpu_error AVkVrSystem::getCurrentTrackedDevicePoseInto ( agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    CHECK_POINTER(dest);
    if(index >= currentTrackedDevicePoses.size())
        return AGPU_INVALID_PARAMETER;

    *dest = currentTrackedDevicePoses[index];
    return AGPU_OK;
}

agpu_size AVkVrSystem::getCurrentRenderTrackedDevicePoseCount ()
{
    return currentRenderTrackedDevicePoses.size();
}

agpu_error AVkVrSystem::getCurrentRenderTrackedDevicePoseInto ( agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    CHECK_POINTER(dest);
    if(index >= currentRenderTrackedDevicePoses.size())
        return AGPU_INVALID_PARAMETER;

    *dest = currentRenderTrackedDevicePoses[index];
    return AGPU_OK;
}

agpu_bool AVkVrSystem::pollEvent ( agpu_vr_event* event )
{
    if(!event)
        return false;

    vr::VREvent_t rawEvent;
    auto device = weakDevice.lock();
    while(deviceForVk->vrSystem->PollNextEvent(&rawEvent, sizeof(rawEvent)))
    {
        // We are using the same ids, so there is no need to map them.
        memset(event, 0, sizeof(agpu_vr_event));
        event->type = rawEvent.eventType;
        event->tracked_device_index = rawEvent.trackedDeviceIndex;
        event->event_age_seconds = rawEvent.eventAgeSeconds;

        switch(rawEvent.eventType)
        {
        case vr::VREvent_TrackedDeviceActivated:
        case vr::VREvent_TrackedDeviceDeactivated:
        case vr::VREvent_TrackedDeviceUpdated:
        case vr::VREvent_TrackedDeviceUserInteractionStarted:
        case vr::VREvent_TrackedDeviceUserInteractionEnded:
        case vr::VREvent_IpdChanged:
        case vr::VREvent_EnterStandbyMode:
        case vr::VREvent_LeaveStandbyMode:
        case vr::VREvent_TrackedDeviceRoleChanged:
        case vr::VREvent_WirelessDisconnect:
        case vr::VREvent_WirelessReconnect:
            return true;

        case vr::VREvent_ButtonPress:
        case vr::VREvent_ButtonUnpress:
        case vr::VREvent_ButtonTouch:
        case vr::VREvent_ButtonUntouch:
            event->data.controller.button = rawEvent.data.controller.button;
            return true;

        case vr::VREvent_DualAnalog_Press:
        case vr::VREvent_DualAnalog_Unpress:
        case vr::VREvent_DualAnalog_Touch:
        case vr::VREvent_DualAnalog_Untouch:
        case vr::VREvent_DualAnalog_Move:
        case vr::VREvent_DualAnalog_ModeSwitch1:
        case vr::VREvent_DualAnalog_ModeSwitch2:
        case vr::VREvent_DualAnalog_Cancel:
            event->data.dual_analog.x = rawEvent.data.dualAnalog.x;
            event->data.dual_analog.y = rawEvent.data.dualAnalog.y;
            event->data.dual_analog.transformed_x = rawEvent.data.dualAnalog.transformedX;
            event->data.dual_analog.transformed_y = rawEvent.data.dualAnalog.transformedY;
            event->data.dual_analog.which = rawEvent.data.dualAnalog.which;
            return true;
        default:
            break;
        }
    }
    return false;
}

// AgpuVkVRSystemSubmissionCommandBuffer
AgpuVkVRSystemSubmissionCommandBuffer::AgpuVkVRSystemSubmissionCommandBuffer()
    : fence(VK_NULL_HANDLE),
      isFenceActive(false),
      commandPool(VK_NULL_HANDLE),
      beforeSubmissionCommandList(VK_NULL_HANDLE),
      afterSubmissionCommandList(VK_NULL_HANDLE),
      leftEyeTexture(nullptr),
      rightEyeTexture(nullptr)
{
}

AgpuVkVRSystemSubmissionCommandBuffer::~AgpuVkVRSystemSubmissionCommandBuffer()
{
    auto device = weakDevice.lock();
    if(device)
    {
        if(fence)
            vkDestroyFence(deviceForVk->device, fence, nullptr);
        if(beforeSubmissionCommandList)
            vkFreeCommandBuffers(deviceForVk->device, commandPool, 1, &beforeSubmissionCommandList);
        if(afterSubmissionCommandList)
            vkFreeCommandBuffers(deviceForVk->device, commandPool, 1, &afterSubmissionCommandList);
        if(commandPool)
            vkDestroyCommandPool(deviceForVk->device, commandPool, nullptr);
    }
}

bool AgpuVkVRSystemSubmissionCommandBuffer::initialize(const agpu::device_ref &device)
{
    this->weakDevice = device;

    // Create the fence
    {
        VkFenceCreateInfo info;
        memset(&info, 0, sizeof(info));
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        auto error = vkCreateFence(deviceForVk->device, &info, nullptr, &fence);
        if (error)
            return false;
    }

    // Create the command pool.
    {
        VkCommandPoolCreateInfo poolCreate;
        memset(&poolCreate, 0, sizeof(poolCreate));
        poolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreate.queueFamilyIndex = deviceForVk->graphicsCommandQueues[0].as<AVkCommandQueue> ()->queueFamilyIndex;
        poolCreate.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        auto error = vkCreateCommandPool(deviceForVk->device, &poolCreate, nullptr, &commandPool);
        if (error)
            return false;
    }

    // Create the before submission command buffer
    {
        VkCommandBufferAllocateInfo commandInfo;
        memset(&commandInfo, 0, sizeof(commandInfo));
        commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandInfo.commandPool = commandPool;
        commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandInfo.commandBufferCount = 1;

        auto error = vkAllocateCommandBuffers(deviceForVk->device, &commandInfo, &beforeSubmissionCommandList);
        if (error)
            return false;
    }

    // Create the after submission command buffer
    {
        VkCommandBufferAllocateInfo commandInfo;
        memset(&commandInfo, 0, sizeof(commandInfo));
        commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandInfo.commandPool = commandPool;
        commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandInfo.commandBufferCount = 1;

        auto error = vkAllocateCommandBuffers(deviceForVk->device, &commandInfo, &afterSubmissionCommandList);
        if (error)
            return false;
    }

    return true;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::beginCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkCommandBufferInheritanceInfo inheritance;
    memset(&inheritance, 0, sizeof(inheritance));
    inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo;
    memset(&beginInfo, 0, sizeof(beginInfo));
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritance;

    auto error = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::waitFence()
{
    if(!isFenceActive)
        return AGPU_OK;

    auto device = weakDevice.lock();
    auto result = vkGetFenceStatus(deviceForVk->device, fence);
    if (result == VK_SUCCESS)
    {
        // Do nothing
    }
    else if (result == VK_NOT_READY)
    {
        auto error = vkWaitForFences(deviceForVk->device, 1, &fence, VK_TRUE, UINT64_MAX);
        CONVERT_VULKAN_ERROR(error);
    }
    else
    {
        CONVERT_VULKAN_ERROR(result);
        return AGPU_ERROR;
    }

    // Reset the fence.
    auto error = vkResetFences(deviceForVk->device, 1, &fence);
    isFenceActive = false;
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::submitVREyeRenderTargets(const agpu::texture_ref &left_eye, const agpu::texture_ref &right_eye)
{
    CHECK_POINTER(left_eye);
    CHECK_POINTER(right_eye);

    // Do the textures have the required image layout? if so, then just submit them.
    auto leftEyeTexture = left_eye.as<AVkTexture> ();
    auto rightEyeTexture = right_eye.as<AVkTexture> ();
    if(leftEyeTexture->description.main_usage_mode == AGPU_TEXTURE_USAGE_COPY_SOURCE &&
       rightEyeTexture->description.main_usage_mode == AGPU_TEXTURE_USAGE_COPY_SOURCE)
        return doSubmissionOfEyeTextures(left_eye, right_eye);

    auto device = weakDevice.lock();

    // Wait for the fence.
    {
        auto error = waitFence();
        if(error)
            return error;
    }

    // Reset the command pool.
    {
        auto error = vkResetCommandPool(deviceForVk->device, commandPool, 0);
        CONVERT_VULKAN_ERROR(error);
    }

    VkImageSubresourceRange imageRange = {};
    imageRange.layerCount = 1;
    imageRange.levelCount = 1;
    imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    // Before submission command buffer
    {
        auto agpuError = beginCommandBuffer(beforeSubmissionCommandList);
        if(agpuError)
            return agpuError;

        if(leftEyeTexture->description.main_usage_mode != AGPU_TEXTURE_USAGE_COPY_SOURCE)
        {
            VkPipelineStageFlags srcStages = 0;
            VkPipelineStageFlags destStages = 0;
            auto barrier = barrierForImageUsageTransition(leftEyeTexture->image, imageRange, leftEyeTexture->description.usage_modes, leftEyeTexture->description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_SOURCE, srcStages, destStages);
            vkCmdPipelineBarrier(beforeSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        if(rightEyeTexture->description.main_usage_mode != AGPU_TEXTURE_USAGE_COPY_SOURCE)
        {
            VkPipelineStageFlags srcStages = 0;
            VkPipelineStageFlags destStages = 0;
            auto barrier = barrierForImageUsageTransition(rightEyeTexture->image, imageRange, rightEyeTexture->description.usage_modes, leftEyeTexture->description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_SOURCE, srcStages, destStages);
            vkCmdPipelineBarrier(beforeSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        auto error = vkEndCommandBuffer(beforeSubmissionCommandList);
        CONVERT_VULKAN_ERROR(error);
    }

    // After submission command buffer
    {
        auto agpuError = beginCommandBuffer(afterSubmissionCommandList);
        if(agpuError)
            return agpuError;

        if(leftEyeTexture->description.main_usage_mode != AGPU_TEXTURE_USAGE_COPY_SOURCE)
        {
            VkPipelineStageFlags srcStages = 0;
            VkPipelineStageFlags destStages = 0;
            auto barrier = barrierForImageUsageTransition(leftEyeTexture->image, imageRange, leftEyeTexture->description.usage_modes, AGPU_TEXTURE_USAGE_COPY_SOURCE, leftEyeTexture->description.main_usage_mode, srcStages, destStages);
            vkCmdPipelineBarrier(beforeSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        if(rightEyeTexture->description.main_usage_mode != AGPU_TEXTURE_USAGE_COPY_SOURCE)
        {
            VkPipelineStageFlags srcStages = 0;
            VkPipelineStageFlags destStages = 0;
            auto barrier = barrierForImageUsageTransition(rightEyeTexture->image, imageRange, rightEyeTexture->description.usage_modes, AGPU_TEXTURE_USAGE_COPY_SOURCE, leftEyeTexture->description.main_usage_mode, srcStages, destStages);
            vkCmdPipelineBarrier(beforeSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        auto error = vkEndCommandBuffer(afterSubmissionCommandList);
        CONVERT_VULKAN_ERROR(error);
    }

    auto graphicsQueue = deviceForVk->graphicsCommandQueues[0].as<AVkCommandQueue> ()->queue;

    // Submit the first command buffer
    {
        VkSubmitInfo submitInfo;
        memset(&submitInfo, 0, sizeof(VkSubmitInfo));
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &beforeSubmissionCommandList;

        auto error = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        CONVERT_VULKAN_ERROR(error);
    }

    // Do the actual submission of the Eye textures to OpenVR.
    {
        auto error = doSubmissionOfEyeTextures(left_eye, right_eye);
        if(error)
            return error;
    }

    // Submit the second command buffer, and signal the fence.
    {
        VkSubmitInfo submitInfo;
        memset(&submitInfo, 0, sizeof(VkSubmitInfo));
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &afterSubmissionCommandList;

        auto error = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
        CONVERT_VULKAN_ERROR(error);

        isFenceActive = true;
    }

    return AGPU_OK;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::doSubmissionOfEyeTextures (const agpu::texture_ref &left_eye, const agpu::texture_ref &right_eye)
{
    leftEyeTexture = left_eye;
    rightEyeTexture = right_eye;

    auto error = submitEyeTexture(vr::Eye_Left, leftEyeTexture);
    if(error)
        return error;

    error = submitEyeTexture(vr::Eye_Right, rightEyeTexture);
    if(error)
        return error;

    return AGPU_OK;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::submitEyeTexture ( vr::Hmd_Eye eye, const agpu::texture_ref &texture)
{
    auto device = weakDevice.lock();
    auto graphicsQueue = deviceForVk->graphicsCommandQueues[0].as<AVkCommandQueue> ();
    auto avkTexture = texture.as<AVkTexture> ();

    vr::VRTextureBounds_t bounds;
    bounds.uMin = 0.0f;
    bounds.uMax = 1.0f;
    bounds.vMin = 0.0f;
    bounds.vMax = 1.0f;

    vr::VRVulkanTextureData_t vulkanData;
    memset(&vulkanData, 0, sizeof(vulkanData));

    vulkanData.m_nImage = ( uint64_t ) avkTexture->image;
	vulkanData.m_pDevice = ( VkDevice_T * ) deviceForVk->device;
	vulkanData.m_pPhysicalDevice = ( VkPhysicalDevice_T * )deviceForVk->physicalDevice;
	vulkanData.m_pInstance = ( VkInstance_T *) deviceForVk->vulkanInstance;
	vulkanData.m_pQueue = ( VkQueue_T * ) graphicsQueue->queue;
    vulkanData.m_nQueueFamilyIndex = graphicsQueue->queueFamilyIndex;

    vulkanData.m_nWidth = avkTexture->description.width;
    vulkanData.m_nHeight = avkTexture->description.height;
	vulkanData.m_nFormat = mapTextureFormat(avkTexture->description.format);
    vulkanData.m_nSampleCount = avkTexture->description.sample_count;

    vr::Texture_t vrTexture= { &vulkanData, vr::TextureType_Vulkan, vr::ColorSpace_Auto };
    vr::VRCompositor()->Submit(eye, &vrTexture, &bounds);

    return AGPU_OK;
}

} // End of namespace AgpuVulkan
