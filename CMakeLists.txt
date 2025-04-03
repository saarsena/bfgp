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
)

# Add tmxparser
add_subdirectory(deps/tmxparser)

# Grab all .cpp source files
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB EXAMPLE_SRC_FILES ${CMAKE_SOURCE_DIR}/examples/*.cpp)

# Main executable (include example sources here)
add_executable(scheduler_demo 
    ${SRC_FILES}
    ${EXAMPLE_SRC_FILES}
)

# Standalone example executables
add_executable(scheduler_example 
    examples/SchedulerExample.cpp
    examples/mains/scheduler_example_main.cpp
)
add_executable(entt_event_example 
    examples/EnttEventExample.cpp
    examples/mains/entt_event_example_main.cpp
)

# Link all necessary libs
target_link_libraries(scheduler_demo
    tmxparser
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Link for the standalone examples
target_link_libraries(scheduler_example
    tmxparser
)
target_link_libraries(entt_event_example
    tmxparser
)
