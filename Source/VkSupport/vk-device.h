//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>
#include "vk-types.h"

#ifdef __cplusplus
extern "C" {
#endif


void vk_device_create
(
    vk_hardware_t in_hardware,

    VkDevice*     out_device,
    VkQueue*      out_queue
);


#ifdef __cplusplus
}
#endif










