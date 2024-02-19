//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vk-device.h"
#include "vk-macros.h"

void vk_device_create
(
    vk_hardware_t in_hardware,

    VkDevice*     out_device,
    VkQueue*      out_queue
)
{
    VkDevice device = 0;
    VkQueue  queue  = 0;

    const char* device_extension_names[] = {
        "VK_KHR_swapchain"
    };
    const uint32_t device_extension_count = 
        sizeof(device_extension_names) / sizeof(char *);

    float priority = 1.0f;

    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueCount = 1;
    queue_info.queueFamilyIndex = in_hardware.queuefamily_index;
    queue_info.pQueuePriorities = &priority;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &queue_info;
    info.enabledExtensionCount = device_extension_count;
    info.ppEnabledExtensionNames = device_extension_names;

    assert
    (
        VK_SUCCESS == 
            vkCreateDevice(in_hardware.physicaldevice, &info, 0, &device)
    );
    vkGetDeviceQueue(device, in_hardware.queuefamily_index, 0, &queue);

    VK_REQSET(out_device, device);
    VK_REQSET(out_queue,  queue);
}










