# CMakeLists.txt — CLion-compatible build for traffic3d.
#
# Commit: build: include src/scene/object.c (was missing → linker error)
#
# Usage:
#   mkdir build && cd build
#   cmake -DCMAKE_BUILD_TYPE=Release ..
#   cmake --build . -j
#   ./traffic3d

cmake_minimum_required(VERSION 3.20)
project(traffic3d C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_C_FLAGS_RELEASE "-O2 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g3 -Wall -Wextra")

find_package(PkgConfig REQUIRED)
pkg_check_modules(NOTCURSES REQUIRED notcurses)
find_library(MATH_LIB m)

add_executable(traffic3d
        src/main.c
        src/config/config.c
        src/math/vec3.c
        src/math/vec4.c
        src/math/mat4.c
        src/render/framebuffer.c
        src/render/mesh.c
        src/render/rasterizer.c
        src/render/camera.c
        src/render/lighting.c
        src/scene/object.c           # ← THIS LINE WAS MISSING
        src/scene/scene.c
        src/geometry/primitives.c
        src/geometry/road_mesh.c      # <-- NEW
        src/geometry/light_mesh.c     # <-- NEW
        src/traffic/state_machine.c   # <-- NEW
        src/agents/personality.c       # <-- NEW
        src/agents/car_agent.c         # <-- NEW
        src/agents/penguin_agent.c     # <-- NEW
        src/platform/terminal.c
        src/platform/timing.c
        src/platform/sound.c
)

target_include_directories(traffic3d PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${NOTCURSES_INCLUDE_DIRS}
)

target_link_libraries(traffic3d PRIVATE
        ${NOTCURSES_LIBRARIES}
        ${MATH_LIB}
)

target_compile_options(traffic3d PRIVATE ${NOTCURSES_CFLAGS_OTHER})

# Copy the config folder next to the binary so ./traffic3d finds it.
add_custom_command(TARGET traffic3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:traffic3d>/config
)