//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vk-instance.h"
#include "vk-macros.h"
#include "vk-utils.h"

void vk_instance_create
(
    uint32_t           in_vulkan_api_version,
    const char* const* in_required_instance_extension_names,
    uint32_t           in_required_instance_extension_count,

    VkInstance*        out_instance
)
{
    VkInstance instance = 0;

    const char* layer_names[] = {
#ifdef DEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };
    const uint32_t layer_count = sizeof(layer_names) / sizeof(char*);

    uint32_t    instance_extension_count = 
        in_required_instance_extension_count;
    const char* instance_extension_names[64] = {}; 
        //< 64 is magic number to ensure enough space

    memcpy(
        instance_extension_names, 
        in_required_instance_extension_names,
        in_required_instance_extension_count * sizeof(char*)
    );

    instance_extension_names[instance_extension_count++] = 
        "VK_KHR_surface";
#ifdef DEBUG
    instance_extension_names[instance_extension_count++] = 
        "VK_EXT_debug_utils";
#endif

    VkApplicationInfo app = {};
    app.sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.apiVersion = in_vulkan_api_version;

    VkInstanceCreateInfo info = {};
    info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo        = &app;
    info.ppEnabledExtensionNames = instance_extension_names;
    info.enabledExtensionCount   = instance_extension_count;
    info.enabledLayerCount       = layer_count;
    info.ppEnabledLayerNames     = layer_names;
#ifdef DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_info = 
        vk_utils_get_debug_utils_msgr_infoex();
    info.pNext = &debug_info;
#endif

    assert(VK_SUCCESS == vkCreateInstance(&info, 0, &instance));

    VK_REQSET(out_instance, instance);
}










