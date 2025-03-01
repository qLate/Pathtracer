
cmake_minimum_required(VERSION 3.18)
set(CXX_STANDARD 20)
set(PROFILING TRUE)
set(USING_VISUAL_STUDIO TRUE)
set(ENABLE_FAST_MATH ON)
project("Pathtracer")

include(FetchContent)
include_directories(include)
include_directories(include/Math)
include_directories(include/Object)
include_directories(include/Object/GraphicalObject)
include_directories(include/Raycast)
include_directories(include/ObjectAppearance)
include_directories(include/Physics)
include_directories(include/shaders)
include_directories(include/OpenGL)
include_directories(include/System)
include_directories(include/Utils)

include_directories(libs/thread_pool_light)
include_directories(include/UI)

include_directories(libs)
include_directories(libs/SDL2/include)
include_directories(libs/stb_image)
link_directories(libs/SDL2/lib/x64)
enable_testing()

add_executable(${PROJECT_NAME}
        "src/Pathtracer.cpp"
        "src/Object/GraphicalObject.cpp"
        "src/Object/Object.cpp"
        "src/Ray/Raycast.cpp"
        "src/Object/Camera.cpp"
        "src/System/Input.cpp"
        "src/Object/Light.cpp"
        "src/ObjectAppearance/Model.cpp"
        "src/System/BVH.cpp"
        "src/System/Canvas.cpp"
        "src/Object/Triangle.cpp"
        "libs/stb_image/stb_image.h"
        "src/Utils/Utils.cpp"
        "src/System/SDLHandler.cpp"
        "src/System/MyTime.cpp"
        "src/OpenGL/glad.c" 
        "src/OpenGL/Shader.cpp"
        "src/System/Logger.cpp" 
        "src/OpenGL/TraceShader.cpp"
        "src/OpenGL/BufferController.cpp" 
        "src/ObjectAppearance/Material.cpp"
        "src/OpenGL/GLObject.cpp")

target_link_libraries(${PROJECT_NAME} SDL2main)
target_link_libraries(${PROJECT_NAME} SDL2)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif ()