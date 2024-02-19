//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vk-hardware.h"
#include "vk-macros.h"
#include "vk-utils.h"

VkBool32 vk_hardware_get
(
    VkInstance     in_instance, 
    VkSurfaceKHR   in_surface,

    vk_hardware_t* out_hardware
)
{
    vk_hardware_t hardware = {};

    VK_ENUMERATE
	(
		VkPhysicalDevice, 
		vkEnumeratePhysicalDevices,
		in_instance, 
		physicaldevices_count, 
		physicaldevices, 64
	);

	uint32_t current_score     = 0;
    VkBool32 current_supported = VK_FALSE;

	for (uint32_t i = 0; i < physicaldevices_count; i++) 
	{
		VkPhysicalDeviceProperties physicaldevice_properties = {};
		vkGetPhysicalDeviceProperties
		(
			physicaldevices[i], &physicaldevice_properties
		);
		VK_ENUMERATE(
			VkQueueFamilyProperties,
			vkGetPhysicalDeviceQueueFamilyProperties, 
			physicaldevices[i],
			queuefamily_properties_count, 
			queuefamily_properties, 
			64
		);

		uint32_t score = 
			vk_utils_get_physicaldevicelimits_score
			(
				physicaldevice_properties.limits
			);

		for (uint32_t j = 0; j < queuefamily_properties_count; j++) 
		{
			if 
			(
				(
					queuefamily_properties[j].queueFlags & 
					VK_QUEUE_GRAPHICS_BIT
				) == VK_QUEUE_GRAPHICS_BIT
			) 
			{
				VkBool32 supported = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR
				(
					physicaldevices[i], j,
					in_surface, 
					&supported
				);

				if (supported && (score > current_score)) 
				{
					hardware.physicaldevice = physicaldevices[i];
					hardware.queuefamily_index = j;
					hardware.physicaldevice_properties = 
						physicaldevice_properties;
					vkGetPhysicalDeviceMemoryProperties
					(
						hardware.physicaldevice,
						&hardware.physicaldevice_memory_properties
					);
                    vkGetPhysicalDeviceSurfaceCapabilitiesKHR
					(
                        hardware.physicaldevice,
                        in_surface,
                        &hardware.surface_capabilities
                    );

					current_score     = score;
                    current_supported = supported;
				}
			}
		}
	}

    VK_REQSET(out_hardware, hardware);

    return (current_score > 0) && (current_supported == VK_TRUE);
}










