//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "vk-utils.h"
#include "vk-macros.h"

static size_t get_filesize(FILE* fp)
{
    size_t current_pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, current_pos, SEEK_SET);
    return size;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_msgr_callback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity, 
    VkDebugUtilsMessageTypeFlagsEXT             messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
    void*                                       pUserData
) 
{
    (void)messageSeverity;
    (void)messageType;
    (void)pUserData;

    printf("%s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT vk_utils_get_debug_utils_msgr_infoex() 
{
    VkDebugUtilsMessengerCreateInfoEXT info = {};
    info.sType           = 
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity = 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType     = 
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = debug_utils_msgr_callback;
    return info;
}

uint32_t vk_utils_get_physicaldevicelimits_score(VkPhysicalDeviceLimits limits) 
{
    uint32_t score = 0;
    score += limits.maxImageDimension1D;
    score += limits.maxImageDimension2D;
    score += limits.maxImageDimension3D;
    score += limits.maxImageDimensionCube;
    score += limits.maxImageArrayLayers;
    score += limits.maxTexelBufferElements;
    score += limits.maxUniformBufferRange;
    score += limits.maxStorageBufferRange;
    score += limits.maxPushConstantsSize;
    score += limits.maxMemoryAllocationCount;
    score += limits.maxSamplerAllocationCount;
    score += limits.maxBoundDescriptorSets;
    score += limits.maxPerStageDescriptorSamplers;
    score += limits.maxPerStageDescriptorUniformBuffers;
    score += limits.maxPerStageDescriptorStorageBuffers;
    score += limits.maxPerStageDescriptorSampledImages;
    score += limits.maxPerStageDescriptorStorageImages;
    score += limits.maxPerStageDescriptorInputAttachments;
    score += limits.maxPerStageResources;
    score += limits.maxDescriptorSetSamplers;
    score += limits.maxDescriptorSetUniformBuffers;
    score += limits.maxDescriptorSetUniformBuffersDynamic;
    score += limits.maxDescriptorSetStorageBuffers;
    score += limits.maxDescriptorSetStorageBuffersDynamic;
    score += limits.maxDescriptorSetSampledImages;
    score += limits.maxDescriptorSetStorageImages;
    score += limits.maxDescriptorSetInputAttachments;
    score += limits.maxVertexInputAttributes;
    score += limits.maxVertexInputBindings;
    score += limits.maxVertexInputAttributeOffset;
    score += limits.maxVertexInputBindingStride;
    score += limits.maxVertexOutputComponents;
    score += limits.maxTessellationGenerationLevel;
    score += limits.maxTessellationPatchSize;
    score += limits.maxTessellationControlPerVertexInputComponents;
    score += limits.maxTessellationControlPerVertexOutputComponents;
    score += limits.maxTessellationControlPerPatchOutputComponents;
    score += limits.maxTessellationControlTotalOutputComponents;
    score += limits.maxTessellationEvaluationInputComponents;
    score += limits.maxTessellationEvaluationOutputComponents;
    score += limits.maxGeometryShaderInvocations;
    score += limits.maxGeometryInputComponents;
    score += limits.maxGeometryOutputComponents;
    score += limits.maxGeometryOutputVertices;
    score += limits.maxGeometryTotalOutputComponents;
    score += limits.maxFragmentInputComponents;
    score += limits.maxFragmentOutputAttachments;
    score += limits.maxFragmentDualSrcAttachments;
    score += limits.maxFragmentCombinedOutputResources;
    score += limits.maxComputeSharedMemorySize;
    score += limits.maxComputeWorkGroupInvocations;
    score += limits.maxDrawIndexedIndexValue;
    score += limits.maxDrawIndirectCount;
    score += limits.maxSamplerLodBias;
    score += limits.maxSamplerAnisotropy;
    score += limits.maxViewports;
    score += limits.maxTexelOffset;
    score += limits.maxTexelGatherOffset;
    score += limits.maxInterpolationOffset;
    score += limits.maxFramebufferWidth;
    score += limits.maxFramebufferHeight;
    score += limits.maxFramebufferLayers;
    score += limits.framebufferColorSampleCounts;
    score += limits.framebufferDepthSampleCounts;
    score += limits.framebufferStencilSampleCounts;
    score += limits.framebufferNoAttachmentsSampleCounts;
    score += limits.maxColorAttachments;
    score += limits.sampledImageColorSampleCounts;
    score += limits.sampledImageIntegerSampleCounts;
    score += limits.sampledImageDepthSampleCounts;
    score += limits.sampledImageStencilSampleCounts;
    score += limits.storageImageSampleCounts;
    score += limits.maxSampleMaskWords;
    score += limits.maxClipDistances;
    score += limits.maxCullDistances;
    score += limits.maxCombinedClipAndCullDistances;

    return score;
}

VkSurfaceFormatKHR vk_utils_get_format
(
    vk_hardware_t hardware, 
    VkSurfaceKHR  surface,
    VkFormat      requested
) 
{
    VK_ENUMERATE2(
        VkSurfaceFormatKHR,
        vkGetPhysicalDeviceSurfaceFormatsKHR,
        hardware.physicaldevice,
        surface,
        formats_count,
        formats,
        64 //magic number large enough for array of all possibilities
    );

    for (uint32_t i = 0; i < formats_count; i++)
    {
        if (requested == formats[i].format)
            return formats[i];
    }

    return formats[0];
}

uint32_t vk_utils_get_size(vk_hardware_t hardware, uint32_t requested) 
{
    if (hardware.surface_capabilities.maxImageCount == 0) {
        return requested;
    }

    if (requested <= hardware.surface_capabilities.maxImageCount) {
        return requested;
    }

    return hardware.surface_capabilities.minImageCount;
}

VkPresentModeKHR vk_utils_get_mode
(
    vk_hardware_t    hardware, 
    VkSurfaceKHR     surface, 
    VkPresentModeKHR requested
) 
{
    VK_ENUMERATE2
    (
        VkPresentModeKHR,
        vkGetPhysicalDeviceSurfacePresentModesKHR,
        hardware.physicaldevice,
        surface,
        modes_count,
        modes,
        4
    );
    for (uint32_t i = 0; i < modes_count; i++) {
        if (requested == modes[i])
            return requested;
    }

    return modes[0];
}

void vk_utils_create_commandpool_reusable
(
    vk_hardware_t  in_hardware,
    VkDevice       in_device,

    VkCommandPool* out_commandpool
)
{
    VkCommandPool commandpool = 0;

    VkCommandPoolCreateInfo info = {};
    info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags            = 
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | 
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = in_hardware.queuefamily_index;

    assert
    (
        VK_SUCCESS == 
        vkCreateCommandPool(in_device, &info, 0, &commandpool)
    );

    VK_REQSET(out_commandpool, commandpool); 
}

void vk_utils_create_commandbuffers
(
    VkDevice          in_device,
    VkCommandPool     in_commandpool,
    uint8_t           in_count,

    VkCommandBuffer** out_commandbuffers,

    uint8_t           in_copy_count,         
    ...
)  
{
    assert(in_count);

    VkCommandBuffer* commandbuffers = 
        malloc(sizeof(VkCommandBuffer) * in_count);

    VkCommandBufferAllocateInfo info = {};
    info.sType              = 
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool        = in_commandpool;
    info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = in_count;

    assert
    (
        VK_SUCCESS == 
        vkAllocateCommandBuffers(in_device, &info, commandbuffers)
    );

    VK_SET(out_commandbuffers, commandbuffers);

    if(in_copy_count)
    {
        assert(in_copy_count <= in_count);
        va_list args;
        va_start(args, in_copy_count);
        for(uint8_t i = 0; i < in_copy_count; i++)
        {
            VkCommandBuffer* buffer_ptr = va_arg(args, VkCommandBuffer*);
            *buffer_ptr = commandbuffers[i];
        }
    }

    if(NULL == out_commandbuffers)
    {
        free(commandbuffers);
    }
}

VkShaderModule vk_utils_create_shadermodule
(
    VkDevice device, 
    const char* path
) 
{
    VkShaderModule shadermodule = VK_NULL_HANDLE;

    FILE* fp = fopen(path, "rb");

    if(fp) {
        const uint32_t size = get_filesize(fp);
        uint32_t* buffer    = 
            calloc(size / sizeof(uint32_t) + 1, sizeof(uint32_t));
        memset(buffer, 0, size);
        int ret = fread(buffer, size, 1, fp);
        (void)ret;

        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = size;
        info.pCode = buffer;

        assert
        (
            VK_SUCCESS == 
            vkCreateShaderModule(device, &info, 0, &shadermodule)
        );
    }

    return shadermodule;
}

uint32_t vk_utils_get_file_size(FILE* fp) 
{
    const size_t current_position = ftell(fp);
    fseek(fp, 0, SEEK_END);
    const size_t size = ftell(fp);
    fseek(fp, current_position, SEEK_SET);
    return (uint32_t)size;
}

VkFence vk_utils_create_fence(VkDevice device) 
{
    VkFence fence = VK_NULL_HANDLE;

    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    assert(VK_SUCCESS == vkCreateFence(device, &info, 0, &fence));

    return fence;
}

VkSemaphore vk_utils_create_semaphore(VkDevice device) 
{
    VkSemaphore semaphore = VK_NULL_HANDLE;

    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    assert(VK_SUCCESS == vkCreateSemaphore(device, &info, 0, &semaphore));

    return semaphore;
}





