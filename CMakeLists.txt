cmake_minimum_required(VERSION 3.22)

project(VKSUPPORT_SAMPLES 
    VERSION 0.1.0 
    DESCRIPTION "Vulkan Support Library" 
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)

include_directories(Source)

find_package(Vulkan REQUIRED)
find_package(SDL2)

add_subdirectory(Source/VkSupport)

if(SDL2_FOUND)
    add_executable(sdl-sample
        Source/sdl-sample.c
    )
    target_link_libraries(sdl-sample
        PUBLIC VkSupport
        PUBLIC VkSupportSDL
        PUBLIC Vulkan::Vulkan
        PUBLIC SDL2::SDL2
    )
endif()

