
cmake_minimum_required(VERSION 3.18)
set(CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(PROFILING TRUE)
set(USING_VISUAL_STUDIO TRUE)
set(ENABLE_FAST_MATH ON)
project("Pathtracer")

include(FetchContent)
include_directories(include)
include_directories(include/Object)
include_directories(include/OpenGL)
include_directories(include/System)
include_directories(include/Utils)

include_directories(libs)
include_directories(libs/SDL2/include)
include_directories(libs/stb_image)
include_directories(libs/imgui/include)

link_directories(libs/SDL2/lib/x64)
enable_testing()

add_executable(${PROJECT_NAME}
        "src/Program.cpp"
        "src/Object/Graphical.cpp"
        "src/Object/Object.cpp"
        "src/Object/Camera.cpp"
        "src/Object/Light.cpp"
        "src/Object/Triangle.cpp"
        "src/Object/Material.cpp"
        
        "src/System/Input.cpp"
        "src/System/BVH.cpp"
        "src/System/SDLHandler.cpp"
        "src/System/MyTime.cpp"
         
        "src/System/Scene.cpp"
        "src/OpenGL/glad.c"
        "src/OpenGL/Shader.cpp"
        "src/OpenGL/TraceShader.cpp"
        "src/OpenGL/BufferController.cpp" 
        "src/OpenGL/GLObject.cpp"

        "libs/imgui/src/imgui.cpp"
        "libs/imgui/src/imgui_demo.cpp"
        "libs/imgui/src/imgui_draw.cpp"
        "libs/imgui/src/imgui_widgets.cpp"
        "libs/imgui/src/imgui_tables.cpp"
        "libs/imgui/src/imgui_impl_sdl2.cpp"
        "libs/imgui/src/imgui_impl_opengl3.cpp"
 "include/System/ImGUIHandler.h" "src/System/ImGUIHandler.cpp" "include/Utils/Debug.h" "include/Pathtracer.h" "src/Pathtracer.cpp")

target_link_libraries(${PROJECT_NAME} SDL2main)
target_link_libraries(${PROJECT_NAME} SDL2)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif ()