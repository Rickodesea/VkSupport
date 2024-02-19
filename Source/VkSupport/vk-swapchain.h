//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>
#include "vk-types.h"

#ifdef __cplusplus
extern "C" {
#endif


void vk_swapchain_create
(
    vk_hardware_t    in_hardware,
    VkDevice         in_device,
    VkSurfaceKHR     in_surface,
    VkPresentModeKHR in_mode,
    VkFormat         in_format,
    uint32_t         in_size,
    
    VkSwapchainKHR*  out_swapchain,
    VkImage**        out_images,
    VkImageView**    out_imageviews,
    uint32_t*        out_count
);


#ifdef __cplusplus
}
#endif










