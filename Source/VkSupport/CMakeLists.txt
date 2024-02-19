cmake_minimum_required(VERSION 3.22)

project(VKSUPPORT 
    VERSION 0.1.0 
    DESCRIPTION "C99 Vulkan Support Library" 
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)
add_compile_options(-Wall -Werror -Wextra)

find_package(Vulkan REQUIRED)
find_package(SDL2)

add_library(VkSupport SHARED
    vk-types.h
    vk-macros.h
    vk-utils.c
    vk-device.c
    vk-instance.c
    vk-hardware.c
    vk-memory.c
    vk-swapchain.c
)

target_link_libraries(VkSupport
    PUBLIC Vulkan::Vulkan
)

if(SDL2_FOUND)
    add_library(VkSupportSDL SHARED
        vk-sdl.c
    )

    target_link_libraries(VkSupportSDL
        PUBLIC VkSupport
        PUBLIC Vulkan::Vulkan
        PUBLIC SDL2
    )
endif()


