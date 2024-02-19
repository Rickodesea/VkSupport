//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "vk-swapchain.h"
#include "vk-utils.h"
#include "vk-macros.h"

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
) 
{
    VkSwapchainKHR swapchain = 0;

    VkSurfaceFormatKHR surface_format = 
        vk_utils_get_format(in_hardware, in_surface, in_format);

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface          = in_surface;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.minImageCount    = vk_utils_get_size(in_hardware, in_size);
    create_info.imageFormat      = surface_format.format;
    create_info.imageExtent      = 
        in_hardware.surface_capabilities.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageColorSpace  = surface_format.colorSpace;
    create_info.preTransform     = 
        in_hardware.surface_capabilities.currentTransform;
    create_info.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode      = 
        vk_utils_get_mode(in_hardware, in_surface, in_mode);
    create_info.clipped          = VK_TRUE;

    assert
    (
        VK_SUCCESS == 
        vkCreateSwapchainKHR(in_device, &create_info, 0, &swapchain)
    );

    VK_REQSET(out_swapchain, swapchain);

    VK_ENUMERATE2
    (
        VkImage, 
        vkGetSwapchainImagesKHR, 
        in_device, 
        swapchain, 
        images_count, 
        images, 
        16
    );

    VK_SET(out_count, images_count);

    if(out_images)
    {
        *out_images = malloc(sizeof(VkImage) * images_count);
        memcpy(*out_images, images, sizeof(VkImage) * images_count);
    }

    if(out_imageviews)
    {
        *out_imageviews = malloc(sizeof(VkImageView) * images_count);

        for (uint32_t i = 0; i < images_count; i++) 
        {           
            VkImageSubresourceRange subresource = {};
            subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresource.levelCount = 1;
            subresource.layerCount = 1;

            VkImageViewCreateInfo view_create_info = {};
            view_create_info.sType            = 
                VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_create_info.image            = images[i];
            view_create_info.viewType         = VK_IMAGE_VIEW_TYPE_2D;
            view_create_info.format           = surface_format.format;
            view_create_info.subresourceRange = subresource;

            VkImageView imageview = 0;

            assert
            (
                VK_SUCCESS == 
                vkCreateImageView(in_device, &view_create_info, 0, &imageview)
            );

            (*out_imageviews)[i] = imageview;
        }
    }
}











