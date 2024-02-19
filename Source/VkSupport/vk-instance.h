//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif


void vk_instance_create
(
    uint32_t           in_vulkan_api_version,
    const char* const* in_required_instance_extension_names,
    uint32_t           in_required_instance_extension_count,

    VkInstance*        out_instance
);


#ifdef __cplusplus
}
#endif










