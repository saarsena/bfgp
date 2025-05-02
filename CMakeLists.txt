cmake_minimum_required(VERSION 3.16)
project(scheduler_demo VERSION 0.1 LANGUAGES C CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable warnings for GCC/Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find SDL2 and extensions
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# Include SDL and EnTT
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/externals/imgui
    ${CMAKE_SOURCE_DIR}/externals/imgui/backends
    ${CMAKE_SOURCE_DIR}/externals
)

# Add ImGui file browser
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/imgui-filebrowser
)

# Add tmxparser
add_subdirectory(deps/tmxparser)

# Grab all .cpp source files
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB EXAMPLE_SRC_FILES ${CMAKE_SOURCE_DIR}/examples/*.cpp)

# Main executable (include example sources here)
# add_executable(scheduler_demo 
#     ${SRC_FILES}
#     ${EXAMPLE_SRC_FILES}
#     externals/imgui/imgui.cpp
#     externals/imgui/imgui_demo.cpp
#     externals/imgui/imgui_draw.cpp
#     externals/imgui/imgui_tables.cpp
#     externals/imgui/imgui_widgets.cpp
#     externals/imgui/backends/imgui_impl_sdl2.cpp
#     externals/imgui/backends/imgui_impl_sdlrenderer2.cpp
# )

# Copy assets directory to build directory
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# # Standalone example executables
# add_executable(scheduler_example 
#     examples/SchedulerExample.cpp
#     examples/mains/scheduler_example_main.cpp
# )
# add_executable(entt_event_example 
#     examples/EnttEventExample.cpp
#     examples/mains/entt_event_example_main.cpp
# )

# # Add dungeon example
# add_executable(dungeon_example
#     examples/DungeonExample.hpp
#     examples/mains/dungeon_example_main.cpp
#     src/walkerdungeon.cpp
# )

# Add collision test executable
add_executable(collision_test
    examples/CollisionTest.cpp
    include/examples/CollisionTest.hpp
    examples/mains/collision_test_main.cpp
    src/TmxMapLoader.cpp
    src/Mob.cpp
    src/walkerdungeon.cpp
    src/GameManager.cpp
    src/Player.cpp
    src/core/WalkerDungeonGenerator.cpp
    src/core/EntityFactory.cpp
    src/core/GraphicsContext.cpp
    src/core/WanderSystem.cpp
    src/ui/PerformanceWindow.cpp
    src/ui/EntityInspectorWindow.cpp
    src/ui/TmxLoaderWindow.cpp
    src/ui/WalkerDungeonWindow.cpp
    src/rendering/Renderer.cpp
    src/input/InputHandler.cpp
    externals/imgui/imgui.cpp
    externals/imgui/imgui_demo.cpp
    externals/imgui/imgui_draw.cpp
    externals/imgui/imgui_tables.cpp
    externals/imgui/imgui_widgets.cpp
    externals/imgui/backends/imgui_impl_sdl2.cpp
    externals/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# Copy assets to build directory
add_custom_command(TARGET collision_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# # Link all necessary libs
# target_link_libraries(scheduler_demo
#     tmxparser
#     ${SDL2_LIBRARIES}
#     ${SDL2_IMAGE_LIBRARIES}
#     ${SDL2_TTF_LIBRARIES}
#     ${SDL2_MIXER_LIBRARIES}
# )

# # Link for the standalone examples
# target_link_libraries(scheduler_example
#     tmxparser
# )
# target_link_libraries(entt_event_example
#     tmxparser
# )
# target_link_libraries(dungeon_example
#     tmxparser
#     ${SDL2_LIBRARIES}
#     ${SDL2_IMAGE_LIBRARIES}
#     ${SDL2_TTF_LIBRARIES}
#     ${SDL2_MIXER_LIBRARIES}
# )

target_link_libraries(collision_test
    tmxparser
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)
