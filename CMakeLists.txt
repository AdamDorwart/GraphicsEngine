cmake_minimum_required(VERSION 3.0)
project(MeshSimplification)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(glfw3 REQUIRED)
include_directories(${glfw3_INCLUDE_DIRS})
link_directories(${glfw3_LIBRARY_DIRS})
add_definitions(${glfw3_DEFINITIONS})
if(NOT glfw3_FOUND)
    message(ERROR " glfw3 not found!")
endif(NOT glfw3_FOUND)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(${GLEW_DEFINITIONS})
if(NOT GLEW_FOUND)
    message(ERROR " GLEW not found!")
endif(NOT GLEW_FOUND)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

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

