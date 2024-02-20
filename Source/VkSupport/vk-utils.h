//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>
#include "vk-types.h"

#ifdef __cplusplus
extern "C" {
#endif


VkDebugUtilsMessengerCreateInfoEXT vk_utils_get_debug_utils_msgr_infoex();
uint32_t vk_utils_get_physicaldevicelimits_score(VkPhysicalDeviceLimits limits);
VkSurfaceFormatKHR vk_utils_get_format(
    vk_hardware_t hardware, 
    VkSurfaceKHR  surface,
    VkFormat      requested
);
uint32_t vk_utils_get_size(vk_hardware_t hardware, uint32_t requested);
VkPresentModeKHR vk_utils_get_mode
(
    vk_hardware_t    hardware, 
    VkSurfaceKHR     surface, 
    VkPresentModeKHR requested
);
void vk_utils_create_commandpool_reusable(
    vk_hardware_t  in_hardware,
    VkDevice       in_device,

    VkCommandPool* out_commandpool
);
void vk_utils_create_commandbuffers
(
    VkDevice          in_device,
    VkCommandPool     in_commandpool,
    uint8_t           in_count,

    VkCommandBuffer** out_commandbuffers,

    uint8_t           in_copy_count,         
    ...
);
VkShaderModule vk_utils_create_shadermodule(VkDevice device, const char* path);
uint32_t vk_utils_get_file_size(FILE* fp);

VkSemaphore vk_utils_create_semaphore(VkDevice device);
VkFence vk_utils_create_fence(VkDevice device);

VkBool32 vk_utils_fence_signaled
(
    VkDevice       in_device, 
    VkFence        in_fence,
    VkBool32       in_reset
);

const char* vk_utils_result_string(VkResult res) ;

#ifdef __cplusplus
}
#endif










