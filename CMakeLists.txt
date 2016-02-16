cmake_minimum_required(VERSION 3.0)
project(MeshSimplification)

link_libraries(m glfw3 GLEW GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor dl pthread)

set(SOURCES 
	src/main.cpp
	src/Logger.cpp
	src/Util.cpp
	src/Matrix.cpp
	src/Window.cpp
	src/Shader.cpp
	src/Mesh.cpp)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
add_executable(program ${SOURCES})

