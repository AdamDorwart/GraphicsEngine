cmake_minimum_required(VERSION 3.0)
project(MeshSimplification)

link_libraries(m glfw3 GLEW GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor dl pthread)

include_directories(include)

add_definitions(-DLOG_FILE="log.txt"
				-DSHADER_PATH="../src/shaders/"
				-DMODEL_PATH="../media/")

set(SOURCES 
	src/main.cpp
	src/Logger.cpp
	src/Util.cpp
	src/Window.cpp
	src/Shader.cpp
	src/Mesh.cpp
	src/CoordFrame.cpp
	src/SimpleShader.cpp)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
add_executable(program ${SOURCES})

