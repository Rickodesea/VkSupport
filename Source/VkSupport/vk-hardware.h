//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>
#include "vk-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Selects a hardware that supports Graphical Rendering.
/// @return VK_TRUE if hardware found that meets all the requirements
VkBool32 vk_hardware_get(
    VkInstance     in_instance, 
    VkSurfaceKHR   in_surface,

    vk_hardware_t* out_hardware
);


#ifdef __cplusplus
}
#endif










