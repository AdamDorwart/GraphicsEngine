cmake_minimum_required(VERSION 3.0)
project(MeshSimplification)

link_libraries(m glfw3 GLEW GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor dl pthread)

add_executable(program src/main.cpp src/Logger.cpp src/Window.cpp src/Shader.cpp src)
