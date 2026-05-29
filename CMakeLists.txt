cmake_minimum_required(VERSION 3.15)
project(App LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

message("-- Toolchain file: " ${CMAKE_TOOLCHAIN_FILE})

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(App
        src/main.c

        src/core/mem.c
        src/core/log.c
        src/core/file.c
        src/core/path.c

        src/math/math.c

        src/platform/window.c

        src/render/gl.c
        src/render/shader.c
        src/render/renderer.c
        src/render/texture.c
        src/render/mesh.c
)

target_include_directories(App SYSTEM PRIVATE third_party)

find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Stb REQUIRED)
target_include_directories(App SYSTEM PRIVATE ${Stb_INCLUDE_DIR})
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")
target_include_directories(App SYSTEM PRIVATE ${CGLTF_INCLUDE_DIRS})
target_link_libraries(App PRIVATE Threads::Threads glfw OpenGL::GL)

#add_custom_command(TARGET App POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/assets
#        $<TARGET_FILE_DIR:App>/assets
#        COMMENT "Copying assets to output folder"
#)

add_custom_command(TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/")

