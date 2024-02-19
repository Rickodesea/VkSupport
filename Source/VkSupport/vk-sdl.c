//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <SDL2/SDL_vulkan.h>

#include "vk-sdl.h"
#include "vk-macros.h"
#include "vk-instance.h"
#include "vk-hardware.h"
#include "vk-device.h"

void vk_sdl_create
(
    SDL_Window*    in_window,
    uint32_t       in_vulkan_api_version,

    VkInstance*    out_instance,
    VkSurfaceKHR*  out_suface,
    VkDevice*      out_device,
    VkQueue*       out_queue,
    vk_hardware_t* out_hardware
)
{
    assert(in_window);

    VkInstance    instance =  0;
    VkSurfaceKHR  surface  =  0;
    VkDevice      device   =  0;
    VkQueue       queue    =  0;
    vk_hardware_t hardware = {};

    uint32_t     instance_extension_count = 0;
    const char** instance_extension_names = 0;

    assert(
        vk_sdl_get_required_instance_extensions
        (
            in_window, 
            &instance_extension_count, 
            &instance_extension_names
        )
    );
    
    vk_instance_create
    (
        in_vulkan_api_version, 
        instance_extension_names, 
        instance_extension_count,
        &instance
    );

    vk_sdl_create_surface(in_window, instance, &surface);

    assert(VK_TRUE == vk_hardware_get(instance, surface, &hardware));

    vk_device_create(hardware, &device, &queue);

    VK_REQSET(out_instance, instance);
    VK_REQSET(out_suface,   surface );
    VK_REQSET(out_device,   device  );
    VK_REQSET(out_queue,    queue   );
    VK_REQSET(out_hardware, hardware);

    VK_FREE(instance_extension_names);
}

SDL_bool vk_sdl_get_required_instance_extensions
(
    SDL_Window*   in_window,

    uint32_t*     inout_count,
    const char*** inout_names
)
{
    assert(in_window);

    uint32_t lcount = 0;
    SDL_Vulkan_GetInstanceExtensions(in_window, &lcount, 0);
    if(inout_count)
    {
        if(0 == *inout_count || *inout_count > lcount) *inout_count = lcount;
        else lcount = *inout_count;

        if(inout_names)
        {
            if(0 == *inout_names)
            {
                *inout_names = malloc(lcount * sizeof(char*));
            }

            return 
                SDL_Vulkan_GetInstanceExtensions
                (
                    in_window, 
                    &lcount, 
                    *inout_names
                );
        }
    }

    return SDL_FALSE;
}

void vk_sdl_create_surface
(
    SDL_Window*   in_window, 
    VkInstance    in_instance,

    VkSurfaceKHR* out_surface
) 
{
    assert(in_window);
    assert(in_instance);

    assert
    (
        (SDL_GetWindowFlags(in_window) & SDL_WINDOW_VULKAN) == 
        SDL_WINDOW_VULKAN
    );

    VkSurfaceKHR surface = 0;

    assert
    (
        SDL_TRUE == 
        SDL_Vulkan_CreateSurface(in_window, in_instance, &surface)
    );

    VK_SET(out_surface, surface);
}











