//Copyright (C) 2024 Alrick Grandison, Algodal™

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "VkSupport/vk-sdl.h"
#include "VkSupport/vk-swapchain.h"
#include "VkSupport/vk-utils.h"
#include "VkSupport/vk-macros.h"

#define WIDTH  600
#define HEIGHT 600

#define FRAME_TICKS (1000 / 60)

static void CreateRenderpass
(
    vk_hardware_t in_hardware,
    VkDevice      in_device,
    VkSurfaceKHR  in_surface,
    VkFormat      in_format,

    VkRenderPass* out_renderpass
);

static void CreatePipeline
(
    VkDevice         in_device,
    VkPipelineLayout in_pipeline_layout,
    VkRenderPass     in_renderpass,
    const char*      in_vertex,
    const char*      in_fragment,

    VkPipeline*      out_pipeline
);

static void CreateLayout
(
    VkDevice               in_device,

    VkDescriptorSetLayout* out_descriptorset_layout,
    VkPipelineLayout*      out_pipeline_layout
);

static void CreateFramebuffers
(
    VkDevice        in_device,
    VkRenderPass    in_renderpass,
    uint32_t        in_width,
    uint32_t        in_height,
    uint32_t        in_count,
    VkImageView*    in_imageviews,

    VkFramebuffer** out_framebuffers
);

static void Render
(
    vk_hardware_t hardware,
    VkDevice device,
    VkQueue queue,
    VkSwapchainKHR swapchain,
    VkRenderPass renderpass,
    VkFramebuffer* framebuffers,
    VkPipeline pipeline,
    VkCommandBuffer commandbuffer,
    VkFence fence_acquire,
    VkFence fence_queue
);

int main() 
{
    if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        SDL_Log("Failed SDL Initialization");
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow(
        "Hello World!",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
    );

    if( window == NULL ) {
        SDL_Log("Failed SDL Window Creation");
        exit(EXIT_FAILURE);
    }

    SDL_Event event   = {};
    bool  running = true;
    uint64_t previous_ticks = SDL_GetTicks64(), current_ticks;
    int64_t  accum_ticks = 0;

    VkInstance    instance;
    VkSurfaceKHR  surface;
    VkDevice      device;
    VkQueue       queue;
    vk_hardware_t hardware;

    vk_sdl_create(
        window,
        VK_API_VERSION_1_3,
        &instance,
        &surface,
        &device,
        &queue,
        &hardware
    );

    VkSwapchainKHR swapchain = 0;
    VkImageView*   swapchain_views = 0;
    uint32_t       swapchain_size = 0;

    vk_swapchain_create
    (
        hardware,
        device,
        surface,
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_FORMAT_R8G8B8_UNORM,
        4,
        &swapchain,
        NULL,
        &swapchain_views,
        &swapchain_size
    );

    VkCommandPool commandpool;

    vk_utils_create_commandpool_reusable
    (
        hardware,
        device,
        &commandpool
    );

    VkCommandBuffer commandbuffer_draw;

    vk_utils_create_commandbuffers
    (
        device,
        commandpool,
        1,
        NULL,
        1,
        &commandbuffer_draw
    );

    VkFence fence_queue   = vk_utils_create_fence(device);
    VkFence fence_acquire = vk_utils_create_fence(device);

    VkRenderPass renderpass;

    CreateRenderpass
    (
        hardware,
        device,
        surface,
        VK_FORMAT_R8G8B8_UNORM,
        &renderpass
    );

    VkFramebuffer* framebuffers = 0;

    CreateFramebuffers
    (
        device,
        renderpass,
        WIDTH,
        HEIGHT,
        swapchain_size,
        swapchain_views,
        &framebuffers
    );

    VkPipelineLayout pipeline_layout;

    CreateLayout
    (
        device,
        NULL,
        &pipeline_layout
    );

    VkPipeline pipeline;

    CreatePipeline
    (
        device,
        pipeline_layout,
        renderpass,
        "main.vert.spv",
        "main.frag.spv",
        &pipeline
    );

    while( running ) 
    {
        while ( SDL_PollEvent( &event ) ) 
        {
            if( event.type == SDL_QUIT ) 
            {
                running = false;
            }
        }

        current_ticks = SDL_GetTicks64();
		const uint64_t elapsed_ticks = current_ticks - previous_ticks;
		previous_ticks = current_ticks;
		accum_ticks += elapsed_ticks;

        while ( accum_ticks >= FRAME_TICKS ) 
        {
            accum_ticks -= FRAME_TICKS;

            Render
            (
                hardware,
                device,
                queue,
                swapchain,
                renderpass,
                framebuffers,
                pipeline,
                commandbuffer_draw,
                fence_acquire,
                fence_queue
            );
		}
    }

    
    vkDestroyPipeline(device, pipeline, NULL);
    vkDestroyPipelineLayout(device, pipeline_layout, NULL);
    for(uint32_t i = 0; i < swapchain_size; i++)
    {
        vkDestroyImageView(device, swapchain_views[i], NULL);
        vkDestroyFramebuffer(device, framebuffers[i], NULL);
    }
    free(framebuffers);
    vkDestroyRenderPass(device, renderpass, NULL);
    vkDestroyFence(device, fence_acquire, 0);
    vkDestroyFence(device, fence_queue, 0);
    vkDestroySwapchainKHR(device, swapchain, 0);
    vkDestroyCommandPool(device, commandpool, 0);
    vkDestroyDevice(device, 0);
    vkDestroySurfaceKHR(instance, surface, 0);
    vkDestroyInstance(instance, 0);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

static void CreateRenderpass
(
    vk_hardware_t in_hardware,
    VkDevice      in_device,
    VkSurfaceKHR  in_surface,
    VkFormat      in_format,

    VkRenderPass* out_renderpass
) 
{
    VkRenderPass renderpass = 0;

    VkSurfaceFormatKHR surface_format = 
        vk_utils_get_format(in_hardware, in_surface, in_format);

    VkAttachmentDescription present_attachment_desc = {};
    present_attachment_desc.format = surface_format.format;
    present_attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
    present_attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    present_attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    present_attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    present_attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    present_attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    present_attachment_desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription descriptions[] = {
        present_attachment_desc
    };

    VkAttachmentReference present_attachment_ref = {};
    present_attachment_ref.attachment = 0;
    present_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference references[] = {
        present_attachment_ref
    };

    VkSubpassDescription first_subpass = {};
    first_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    first_subpass.colorAttachmentCount = 
        sizeof(references) / sizeof(VkAttachmentReference);
    first_subpass.pColorAttachments = references;   

    VkSubpassDescription subpasses[] = {
        first_subpass
    };

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 
        sizeof(descriptions) / sizeof(VkAttachmentDescription);
    info.pAttachments = descriptions;
    info.subpassCount = sizeof(subpasses) / sizeof(VkSubpassDescription);
    info.pSubpasses = subpasses;

    assert(VK_SUCCESS == vkCreateRenderPass(in_device, &info, 0, &renderpass));

    VK_SET(out_renderpass, renderpass);
}

static void CreatePipeline
(
    VkDevice         in_device,
    VkPipelineLayout in_pipeline_layout,
    VkRenderPass     in_renderpass,
    const char*      in_vertex,
    const char*      in_fragment,

    VkPipeline*      out_pipeline
) 
{
    VkPipeline pipeline = 0;

    VkShaderModule vertex_shadermodule   = 
        vk_utils_create_shadermodule(in_device, in_vertex);
    VkShaderModule fragment_shadermodule = 
        vk_utils_create_shadermodule(in_device, in_fragment);

    VkPipelineShaderStageCreateInfo vertex_create_info = {};
    vertex_create_info.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_create_info.module = vertex_shadermodule;
    vertex_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_create_info = {};
    fragment_create_info.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_create_info.module = fragment_shadermodule;
    fragment_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_create_infos[] = {
        vertex_create_info, 
        fragment_create_info
    };
    const uint32_t shader_create_infos_count = sizeof(shader_create_infos) / sizeof(VkPipelineShaderStageCreateInfo);
    
    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState);
    dynamic_state_create_info.pDynamicStates = dynamic_states;

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
    rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_create_info.cullMode = VK_CULL_MODE_NONE;
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_create_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
    color_blend_attachment_state.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT
    ;
    color_blend_attachment_state.blendEnable = VK_TRUE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment_state;

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.scissorCount = 1;

    VkVertexInputBindingDescription vib_descriptions[1] = {};

    vib_descriptions[0].binding = 0;
    vib_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    vib_descriptions[0].stride = 0;

    VkVertexInputAttributeDescription via_descriptions[5] = {};

    via_descriptions[0].binding = 0;
    via_descriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    via_descriptions[0].location = 0;
    via_descriptions[0].offset = 0;

    via_descriptions[1].binding = 0;
    via_descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    via_descriptions[1].location = 1;
    via_descriptions[1].offset = 1 * sizeof(float) * 4;

    via_descriptions[2].binding = 0;
    via_descriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    via_descriptions[2].location = 2;
    via_descriptions[2].offset = 2 * sizeof(float) * 4;

    via_descriptions[3].binding = 0;
    via_descriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    via_descriptions[3].location = 3;
    via_descriptions[3].offset = 3 * sizeof(float) * 4;

    via_descriptions[4].binding = 0;
    via_descriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    via_descriptions[4].location = 4;
    via_descriptions[4].offset = 4 * sizeof(float) * 4;

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = sizeof(vib_descriptions) / sizeof(VkVertexInputBindingDescription);
    vertex_input_state_create_info.pVertexBindingDescriptions = vib_descriptions;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = sizeof(via_descriptions) / sizeof(VkVertexInputAttributeDescription);
    vertex_input_state_create_info.pVertexAttributeDescriptions = via_descriptions;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
    input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.layout = in_pipeline_layout;
    pipeline_create_info.stageCount = shader_create_infos_count;
    pipeline_create_info.pStages = shader_create_infos;
    pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    pipeline_create_info.renderPass = in_renderpass;
    pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
    pipeline_create_info.pMultisampleState = &multisample_state_create_info;
    pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;

    assert(VK_SUCCESS == vkCreateGraphicsPipelines(in_device, 0, 1, &pipeline_create_info, 0, &pipeline));

    VK_SET(out_pipeline, pipeline);

    vkDestroyShaderModule(in_device, vertex_shadermodule, 0);
    vkDestroyShaderModule(in_device, fragment_shadermodule, 0);
}

static void CreateLayout
(
    VkDevice               in_device,

    VkDescriptorSetLayout* out_descriptorset_layout,
    VkPipelineLayout*      out_pipeline_layout
) 
{
    VkDescriptorSetLayout descriptorset_layout = 0;
    VkPipelineLayout      pipeline_layout      = 0;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    assert(VK_SUCCESS == vkCreatePipelineLayout(in_device, &pipeline_layout_info, 0, &pipeline_layout));

    VK_SET(out_pipeline_layout, pipeline_layout);
}


static void CreateFramebuffers
(
    VkDevice        in_device,
    VkRenderPass    in_renderpass,
    uint32_t        in_width,
    uint32_t        in_height,
    uint32_t        in_count,
    VkImageView*    in_imageviews,

    VkFramebuffer** out_framebuffers
)
{
    assert(out_framebuffers);
    *out_framebuffers = malloc(sizeof(VkFramebuffer) * in_count);
    memset(*out_framebuffers, 0, sizeof(VkFramebuffer) * in_count);
    for (uint32_t i = 0; i < in_count; i++) 
    {
        VkImageView attachments[] = {
            in_imageviews[i]
        };

        VkFramebufferCreateInfo fb_create_info = {};
        fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_create_info.renderPass = in_renderpass;
        fb_create_info.attachmentCount = sizeof(attachments) / sizeof(VkImageView);
        fb_create_info.pAttachments = attachments;
        fb_create_info.width = in_width;
        fb_create_info.height = in_height;
        fb_create_info.layers = 1;

        VkFramebuffer framebuffer = 0;
        
        assert(VK_SUCCESS == vkCreateFramebuffer(
            in_device, &fb_create_info, 0, &framebuffer));
        (*out_framebuffers)[i] = framebuffer;
    }
}

static void Render
(
    vk_hardware_t hardware,
    VkDevice device,
    VkQueue queue,
    VkSwapchainKHR swapchain,
    VkRenderPass renderpass,
    VkFramebuffer* framebuffers,
    VkPipeline pipeline,
    VkCommandBuffer commandbuffer,
    VkFence fence_acquire,
    VkFence fence_queue
)
{
    uint32_t index = 0;
    vkAcquireNextImageKHR(
        device, 
        swapchain, 
        UINT64_MAX, 
        VK_NULL_HANDLE, 
        fence_acquire, 
        &index 
    );

    vkWaitForFences(device, 1, &fence_acquire, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence_acquire);

    VkCommandBufferBeginInfo beginDrawCommandInfo = {};
    beginDrawCommandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginDrawCommandInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkRect2D scissor = {};
    scissor.extent = hardware.surface_capabilities.currentExtent;

    VkViewport viewport = {};
    viewport.width = (float)hardware.surface_capabilities.currentExtent.width;
    viewport.height = (float)hardware.surface_capabilities.currentExtent.height;
    viewport.maxDepth = 1.0f;

    VkClearValue clearValues[1] = {};
    clearValues[0].color.float32[0] = 1.0f; //r
    clearValues[0].color.float32[1] = 1.0f; //g
    clearValues[0].color.float32[2] = 1.0f; //b
    clearValues[0].color.float32[3] = 1.0f; //a

    VkRenderPassBeginInfo beginRenderpassInfo = {};
    beginRenderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginRenderpassInfo.renderPass = renderpass;
    beginRenderpassInfo.framebuffer = framebuffers[index];
    beginRenderpassInfo.renderArea.extent = hardware.surface_capabilities.currentExtent;
    beginRenderpassInfo.clearValueCount = sizeof(clearValues) / sizeof(VkClearValue);
    beginRenderpassInfo.pClearValues = clearValues;

    const VkDeviceSize offset = 0;   
    (void)offset; 

    assert(VK_SUCCESS == vkBeginCommandBuffer(commandbuffer, &beginDrawCommandInfo));
    vkCmdBeginRenderPass(commandbuffer, &beginRenderpassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandbuffer, 0, 1, &scissor); 
    vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    vkCmdEndRenderPass(commandbuffer);
    assert(VK_SUCCESS == vkEndCommandBuffer(commandbuffer));

    VkSubmitInfo drawCommandSubmitInfo = {};
    drawCommandSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    drawCommandSubmitInfo.commandBufferCount = 1;
    drawCommandSubmitInfo.pCommandBuffers = &commandbuffer;

    assert
    (
        VK_SUCCESS == 
        vkQueueSubmit(queue, 1, &drawCommandSubmitInfo, fence_queue)
    );

    vkWaitForFences(device, 1, &fence_queue, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence_queue);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &index;

    assert(VK_SUCCESS == vkQueuePresentKHR(queue, &presentInfo));
}




