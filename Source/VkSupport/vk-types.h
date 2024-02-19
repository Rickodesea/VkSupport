//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vk_hardware_t vk_hardware_t;

struct vk_hardware_t
{
    VkPhysicalDevice                 physicaldevice;
    VkPhysicalDeviceProperties       physicaldevice_properties;
    VkPhysicalDeviceMemoryProperties physicaldevice_memory_properties;
    VkSurfaceCapabilitiesKHR         surface_capabilities;
    uint32_t                         queuefamily_index;
};


#ifdef __cplusplus
}
#endif










