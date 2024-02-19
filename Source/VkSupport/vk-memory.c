//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vk-memory.h"
#include "vk-types.h"
#include "vk-macros.h"

static int32_t get_memory_index(vk_hardware_t hardware, uint32_t requested, VkMemoryPropertyFlags required) 
{
    const uint32_t memoryCount = hardware.physicaldevice_memory_properties.memoryTypeCount;
    for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
        const uint32_t memoryTypeBits = (1 << memoryIndex);
        const VkBool32 isRequiredMemoryType = requested & memoryTypeBits;

        const VkMemoryPropertyFlags in_properties =
            hardware.physicaldevice_memory_properties.memoryTypes[memoryIndex].propertyFlags;
        const VkBool32 hasrequired =
            (in_properties & required) == required;

        if (isRequiredMemoryType && hasrequired)
            return memoryIndex;
    }

    return 0;
}

void vk_memory_create
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size, 
    VkBufferUsageFlags    in_usage, 
    VkMemoryPropertyFlags in_properties,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
) 
{
    VkBuffer       buffer = 0;
    VkDeviceSize   size   = 0;

    VkBufferCreateInfo info = {};
    info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size        = in_size;
    info.usage       = in_usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    assert(VK_SUCCESS == vkCreateBuffer(in_device, &info, 0, &buffer));

    VK_SET(out_buffer, buffer);

    VkMemoryRequirements reqs = {};
    vkGetBufferMemoryRequirements(in_device, buffer, &reqs);
    size = reqs.size;

    VK_SET(out_size, size);

    if(out_memory)
    {
        VkDeviceMemory memory = 0;

        VkMemoryAllocateInfo allocinfo = {};
        allocinfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocinfo.allocationSize  = reqs.size;
        allocinfo.memoryTypeIndex = 
            get_memory_index(in_hardware, reqs.memoryTypeBits, in_properties);

        assert
        (
            VK_SUCCESS == vkAllocateMemory(in_device, &allocinfo, 0, &memory)
        );
        vkBindBufferMemory(in_device, buffer, memory, 0);

        VK_SET(out_memory, memory);
    }
}

void vk_memory_create_staging
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
)
{
    vk_memory_create
    (
        in_hardware,
        in_device, 
        in_size, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        out_buffer,
        out_memory,
        out_size
    );
}

void vk_memory_create_uniform(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
)
{
    vk_memory_create
    (
        in_hardware,
        in_device, 
        in_size, 
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        out_buffer,
        out_memory,
        out_size
    );
}

void vk_memory_create_vertex
(
    vk_hardware_t         in_hardware,
    VkDevice              in_device,
    VkDeviceSize          in_size,

    VkBuffer*             out_buffer,
    VkDeviceMemory*       out_memory,
    VkDeviceSize*         out_size
)
{
    vk_memory_create
    (
        in_hardware,
        in_device, 
        in_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        out_buffer,
        out_memory,
        out_size
    );
}














