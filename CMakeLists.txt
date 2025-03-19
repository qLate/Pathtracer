
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
include_directories(include/BVH)
include_directories(include/UI)
include_directories(include/Ray)
include_directories(include/Other)

include_directories(libs)
include_directories(libs/SDL2/include)
include_directories(libs/glad)
include_directories(libs/stb_image)
include_directories(libs/imgui/include)
include_directories(libs/rapidobj)
include_directories(libs/gl-radix-sort)

link_directories(libs/SDL2/lib/x64)
enable_testing()

add_executable(${PROJECT_NAME}
        "libs/imgui/src/imgui.cpp"
        "libs/imgui/src/imgui_draw.cpp"
        "libs/imgui/src/imgui_widgets.cpp"
        "libs/imgui/src/imgui_tables.cpp"
        "libs/imgui/src/imgui_impl_sdl2.cpp"
        "libs/imgui/src/imgui_impl_opengl3.cpp"

        "src/Program.cpp"

        "src/Object/Graphical.cpp"
        "src/Object/Object.cpp"
        "src/Object/Camera.cpp"
        "src/Object/Light.cpp"
        "src/Object/Triangle.cpp"
        "src/Object/Material.cpp"
        "src/Object/Model.cpp"
        
        "src/System/Input.cpp"
        "src/System/MyTime.cpp"
        "src/System/Renderer.cpp"

        "src/UI/SDLHandler.cpp"
        "src/UI/ImGUIHandler.cpp"
        "src/UI/ImGUIWindowDrawer.cpp"
         
       "libs/glad/glad.c"
        "src/OpenGL/Shader.cpp"
        "src/OpenGL/TraceShader.cpp"
        "src/OpenGL/BufferController.cpp" 
        "src/OpenGL/GLObject.cpp"
        "src/OpenGL/ShaderProgram.cpp"

        "src/BVH/BVH.cpp"
        
        "src/BVH/BVHMortonBuilder.cpp"
        "src/BVH/BVHBasicBuilder.cpp"
        "src/BVH/BVH6SidedBuilder.cpp"
        "src/BVH/BVHSAHBuilder.cpp"

        "src/System/Physics.cpp"

        "src/Other/Scene.cpp"
        "src/Other/Tweener.cpp"

        "src/Utils/MyMath.cpp"
 "include/BVH/BVH6SidedBuilder.h" "include/BVH/MortonCodes.h" "src/BVH/MortonCodes.cpp" "src/Utils/Utils.cpp")

target_link_libraries(${PROJECT_NAME} SDL2main)
target_link_libraries(${PROJECT_NAME} SDL2)

find_package(OpenMP REQUIRED)

if(OpenMP_CXX_FOUND)
    target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_LIBRARIES})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif ()