//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>
#include "vk-types.h"

#ifdef __cplusplus
extern "C" {
#endif


void vk_memory_create
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size, 
    VkBufferUsageFlags    in_usage, 
    VkMemoryPropertyFlags in_properties,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
);

void vk_memory_create_staging
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
);

void vk_memory_create_uniform
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
);

void vk_memory_create_vertex
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
);


#ifdef __cplusplus
}
#endif










