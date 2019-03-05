#include "vr_system.hpp"
#include "command_queue.hpp"
#include "texture.hpp"
#include "texture_format.hpp"

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

_agpu_vr_system::_agpu_vr_system(agpu_device *cdevice)
    : device(cdevice), submissionCommandBufferIndex(0)
{
}

void _agpu_vr_system::lostReferences()
{
}

bool _agpu_vr_system::initialize()
{
    for(auto &submissionCommandBuffer: submissionCommandBuffers)
    {
        if(!submissionCommandBuffer.initialize(device))
            return false;
    }

    return true;
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

    // For vulkan, invert top and bottom.
    frustum->top = bottom;
    frustum->bottom = top;
    return AGPU_OK;
}

agpu_error _agpu_vr_system::submitVREyeRenderTargets ( agpu_texture* left_eye, agpu_texture* right_eye )
{
    std::unique_lock<std::mutex> l(submissionMutex);

    auto error = submissionCommandBuffers[submissionCommandBufferIndex].submitVREyeRenderTargets (left_eye, right_eye);
    if(error)
        return error;

    submissionCommandBufferIndex = (submissionCommandBufferIndex + 1) % FramebufferingSize;
    return AGPU_OK;
}

agpu_vr_tracked_device_pose _agpu_vr_system::convertTrackedDevicePose(agpu_uint deviceId, const vr::TrackedDevicePose_t &devicePose)
{
    agpu_vr_tracked_device_pose convertedPose;
    memset(&convertedPose, 0, sizeof(convertedPose));

    convertedPose.device_id = deviceId;
    convertedPose.device_class = mapTrackedDeviceClass(device->vrSystem->GetTrackedDeviceClass(deviceId));
    convertedPose.device_role = AGPU_VR_TRACKED_DEVICE_ROLE_INVALID;

    convertedPose.device_to_absolute_tracking = convertOVRMatrix(devicePose.mDeviceToAbsoluteTracking);
    convertedPose.velocity = convertOVRVector3(devicePose.vVelocity);
	convertedPose.angular_velocity = convertOVRVector3(devicePose.vAngularVelocity);

    return convertedPose;
}


agpu_error _agpu_vr_system::waitAndFetchVRPoses()
{
    vr::VRCompositor()->WaitGetPoses(trackedDevicesPose, vr::k_unMaxTrackedDeviceCount, renderTrackedDevicesPose, vr::k_unMaxTrackedDeviceCount );


    validTrackedDevicePoses.clear();
    for(size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
    {
        auto &trackedDevicePose = trackedDevicesPose[i];
        if(trackedDevicePose.bPoseIsValid)
            validTrackedDevicePoses.push_back(convertTrackedDevicePose(i, trackedDevicePose));
    }

    validRenderTrackedDevicePoses.clear();
    for(size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
    {
        auto &trackedDevicePose = renderTrackedDevicesPose[i];
        if(trackedDevicePose.bPoseIsValid)
            validRenderTrackedDevicePoses.push_back(convertTrackedDevicePose(i, trackedDevicePose));
    }

    return AGPU_OK;
}

agpu_size _agpu_vr_system::getValidTrackedDevicePoseCount()
{
    return validTrackedDevicePoses.size();
}

agpu_error _agpu_vr_system::getValidTrackedDevicePoseInto ( agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    CHECK_POINTER(dest);
    if(index >= validTrackedDevicePoses.size())
        return AGPU_INVALID_PARAMETER;

    *dest = validTrackedDevicePoses[index];
    return AGPU_OK;
}

agpu_size _agpu_vr_system::getValidRenderTrackedDevicePoseCount ()
{
    return validRenderTrackedDevicePoses.size();
}

agpu_error _agpu_vr_system::getValidRenderTrackedDevicePoseInto ( agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    CHECK_POINTER(dest);
    if(index >= validRenderTrackedDevicePoses.size())
        return AGPU_INVALID_PARAMETER;

    *dest = validRenderTrackedDevicePoses[index];
    return AGPU_OK;
}

// AgpuVkVRSystemSubmissionCommandBuffer
AgpuVkVRSystemSubmissionCommandBuffer::AgpuVkVRSystemSubmissionCommandBuffer()
    : device(nullptr),
      fence(VK_NULL_HANDLE),
      isFenceActive(false),
      commandPool(VK_NULL_HANDLE),
      beforeSubmissionCommandList(VK_NULL_HANDLE),
      afterSubmissionCommandList(VK_NULL_HANDLE),
      leftEyeTexture(nullptr),
      rightEyeTexture(nullptr)
{
}

void AgpuVkVRSystemSubmissionCommandBuffer::lostReferences()
{
    if(fence)
        vkDestroyFence(device->device, fence, nullptr);
    if(beforeSubmissionCommandList)
        vkFreeCommandBuffers(device->device, commandPool, 1, &beforeSubmissionCommandList);
    if(afterSubmissionCommandList)
        vkFreeCommandBuffers(device->device, commandPool, 1, &afterSubmissionCommandList);
    if(commandPool)
        vkDestroyCommandPool(device->device, commandPool, nullptr);
    if(leftEyeTexture)
        leftEyeTexture->release();
    if(rightEyeTexture)
        rightEyeTexture->release();
}

bool AgpuVkVRSystemSubmissionCommandBuffer::initialize(agpu_device *device)
{
    this->device = device;

    // Create the fence
    {
        VkFenceCreateInfo info;
        memset(&info, 0, sizeof(info));
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        auto error = vkCreateFence(device->device, &info, nullptr, &fence);
        if (error)
            return false;
    }

    // Create the command pool.
    {
        VkCommandPoolCreateInfo poolCreate;
        memset(&poolCreate, 0, sizeof(poolCreate));
        poolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreate.queueFamilyIndex = device->graphicsCommandQueues[0]->queueFamilyIndex;
        poolCreate.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        auto error = vkCreateCommandPool(device->device, &poolCreate, nullptr, &commandPool);
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

        auto error = vkAllocateCommandBuffers(device->device, &commandInfo, &beforeSubmissionCommandList);
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

        auto error = vkAllocateCommandBuffers(device->device, &commandInfo, &afterSubmissionCommandList);
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

    auto result = vkGetFenceStatus(device->device, fence);
    if (result == VK_SUCCESS)
    {
        // Do nothing
    }
    else if (result == VK_NOT_READY)
    {
        auto error = vkWaitForFences(device->device, 1, &fence, VK_TRUE, UINT64_MAX);
        CONVERT_VULKAN_ERROR(error);
    }
    else
    {
        CONVERT_VULKAN_ERROR(result);
        return AGPU_ERROR;
    }

    // Reset the fence.
    auto error = vkResetFences(device->device, 1, &fence);
    isFenceActive = false;
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::submitVREyeRenderTargets ( agpu_texture* left_eye, agpu_texture* right_eye )
{
    CHECK_POINTER(left_eye);
    CHECK_POINTER(right_eye);

    // Do the textures have the required image layout? if so, then just submit them.
    if(left_eye->initialLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && right_eye->initialLayout)
        return doSubmissionOfEyeTextures(left_eye, right_eye);

    // Wait for the fence.
    {
        auto error = waitFence();
        if(error)
            return error;
    }

    // Reset the command pool.
    {
        auto error = vkResetCommandPool(device->device, commandPool, 0);
        CONVERT_VULKAN_ERROR(error);
    }

    VkImageSubresourceRange imageRange;
    memset(&imageRange, 0, sizeof(imageRange));
    imageRange.layerCount = 1;
    imageRange.levelCount = 1;

    auto leftSourceLayout = left_eye->initialLayout;
    auto rightSourceLayout = right_eye->initialLayout;

    // Before submission command buffer
    {
        auto agpuError = beginCommandBuffer(beforeSubmissionCommandList);
        if(agpuError)
            return agpuError;

        if(leftSourceLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            auto barrier = device->barrierForImageLayoutTransition(left_eye->image, imageRange, VK_IMAGE_ASPECT_COLOR_BIT, leftSourceLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, left_eye->initialLayoutAccessBits, srcStages, destStages);
            vkCmdPipelineBarrier(beforeSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        if(rightSourceLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            auto barrier = device->barrierForImageLayoutTransition(right_eye->image, imageRange, VK_IMAGE_ASPECT_COLOR_BIT, rightSourceLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, left_eye->initialLayoutAccessBits, srcStages, destStages);
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

        if(leftSourceLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            auto barrier = device->barrierForImageLayoutTransition(left_eye->image, imageRange, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, leftSourceLayout, 0, srcStages, destStages);
            vkCmdPipelineBarrier(afterSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        if(rightSourceLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            auto barrier = device->barrierForImageLayoutTransition(right_eye->image, imageRange, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rightSourceLayout, 0, srcStages, destStages);
            vkCmdPipelineBarrier(afterSubmissionCommandList, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        auto error = vkEndCommandBuffer(afterSubmissionCommandList);
        CONVERT_VULKAN_ERROR(error);
    }

    auto graphicsQueue = device->graphicsCommandQueues[0]->queue;

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

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::doSubmissionOfEyeTextures ( agpu_texture* left_eye, agpu_texture* right_eye )
{
    {
        if(left_eye)
            left_eye->retain();
        if(leftEyeTexture)
            leftEyeTexture->release();
        leftEyeTexture = left_eye;
    }

    {
        if(right_eye)
            right_eye->retain();
        if(rightEyeTexture)
            rightEyeTexture->release();
        rightEyeTexture = right_eye;
    }

    auto error = submitEyeTexture(vr::Eye_Left, leftEyeTexture);
    if(error)
        return error;

    error = submitEyeTexture(vr::Eye_Right, rightEyeTexture);
    if(error)
        return error;

    return AGPU_OK;
}

agpu_error AgpuVkVRSystemSubmissionCommandBuffer::submitEyeTexture ( vr::Hmd_Eye eye, agpu_texture* texture)
{
    auto graphicsQueue = device->graphicsCommandQueues[0];

    vr::VRTextureBounds_t bounds;
    bounds.uMin = 0.0f;
    bounds.uMax = 1.0f;
    bounds.vMin = 0.0f;
    bounds.vMax = 1.0f;

    vr::VRVulkanTextureData_t vulkanData;
    memset(&vulkanData, 0, sizeof(vulkanData));

    vulkanData.m_nImage = ( uint64_t ) texture->image;
	vulkanData.m_pDevice = ( VkDevice_T * ) device->device;
	vulkanData.m_pPhysicalDevice = ( VkPhysicalDevice_T * )device->physicalDevice;
	vulkanData.m_pInstance = ( VkInstance_T *) device->vulkanInstance;
	vulkanData.m_pQueue = ( VkQueue_T * ) graphicsQueue->queue;
    vulkanData.m_nQueueFamilyIndex = graphicsQueue->queueFamilyIndex;

    vulkanData.m_nWidth = texture->description.width;
    vulkanData.m_nHeight = texture->description.height;
	vulkanData.m_nFormat = mapTextureFormat(texture->description.format);
    vulkanData.m_nSampleCount = texture->description.sample_count;

    vr::Texture_t vrTexture= { &vulkanData, vr::TextureType_Vulkan, vr::ColorSpace_Auto };
    vr::VRCompositor()->Submit(eye, &vrTexture, &bounds);

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

AGPU_EXPORT agpu_error agpuSubmitVREyeRenderTargets ( agpu_vr_system* vr_system, agpu_texture* left_eye, agpu_texture* right_eye )
{
    CHECK_POINTER(vr_system);
    return vr_system->submitVREyeRenderTargets(left_eye, right_eye);
}

AGPU_EXPORT agpu_error agpuWaitAndFetchVRPoses ( agpu_vr_system* vr_system )
{
    CHECK_POINTER(vr_system);
    return vr_system->waitAndFetchVRPoses();
}

AGPU_EXPORT agpu_size agpuGetValidVRTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
    if(!vr_system)
        return 0;
    return vr_system->getValidTrackedDevicePoseCount();
}

AGPU_EXPORT agpu_error agpuGetValidVRTrackedDevicePoseInto ( agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    CHECK_POINTER(vr_system);
    return vr_system->getValidTrackedDevicePoseInto(index, dest);
}

AGPU_EXPORT agpu_size agpuGetValidVRRenderTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
    if(!vr_system)
        return 0;
    return vr_system->getValidRenderTrackedDevicePoseCount();
}

AGPU_EXPORT agpu_error agpuGetValidVRRenderTrackedDevicePoseInto ( agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest )
{
    CHECK_POINTER(vr_system);
    return vr_system->getValidRenderTrackedDevicePoseInto(index, dest);
}
