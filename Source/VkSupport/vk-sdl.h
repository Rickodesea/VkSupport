//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include "vk-types.h"

#ifdef __cplusplus
extern "C" {
#endif


void vk_sdl_create
(
    SDL_Window*    in_window,
    uint32_t       in_vulkan_api_version,

    VkInstance*    out_instance,
    VkSurfaceKHR*  out_suface,
    VkDevice*      out_device,
    VkQueue*       out_queue,
    vk_hardware_t* out_hardware
);

SDL_bool vk_sdl_get_required_instance_extensions
(
    SDL_Window*   in_window,

    uint32_t*     inout_count,
    const char*** inout_names
);

void vk_sdl_create_surface
(
    SDL_Window*   in_window, 
    VkInstance    in_instance,

    VkSurfaceKHR* out_surface
);


#ifdef __cplusplus
}
#endif










