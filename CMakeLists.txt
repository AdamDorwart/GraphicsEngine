cmake_minimum_required(VERSION 3.0)
project(MeshSimplification)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

link_libraries(m glfw3 GLEW GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor dl pthread)

include_directories(include)

# Compiler arguments
add_definitions(-Wall -std=c++11)

# Compiler macro definitions
add_definitions(-DLOG_FILE="log.txt"
				-DSHADER_PATH="../src/shaders/"
				-DMODEL_PATH="../media/")

set(SOURCES 
	src/main.cpp
	src/Logger.cpp
	src/Util.cpp
	src/Window.cpp
	src/InputHandler.cpp
	src/RenderPipeline.cpp
	src/Shader.cpp
	src/SceneGraph.cpp
	src/Mesh.cpp
	src/CoordFrame.cpp
	src/FlatShader.cpp
	src/NormalShader.cpp)

add_executable(program ${SOURCES})

