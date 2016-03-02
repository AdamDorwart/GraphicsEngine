cmake_minimum_required(VERSION 3.0)
project(GraphicsEngine)

set(APP_NAME GraphicsEngine)

set(GLFW_SOURCE_DIR "external/glfw")
set(GLM_SOURCE_DIR "external/glm")
set(GLEW_SOURCE_DIR "external/glew")

# Building only the GLFW lib
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

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

find_package(OpenGL REQUIRED)

# Compiler arguments
add_definitions(-Wall -std=c++11)

# Compiler macro definitions
add_definitions(-DLOG_FILE="log.txt"
                -DSHADER_PATH="../src/shaders/"
                -DMODEL_PATH="../media/")

add_subdirectory(${GLFW_SOURCE_DIR} ${GLEW_SOURCE_DIR})

include_directories(include
                    ${GLFW_SOURCE_DIR}/include
                    ${GLFW_SOURCE_DIR}/deps
                    ${GLM_SOURCE_DIR}
                    ${GLEW_SOURCE_DIR}/include)

add_library(glew STATIC
  ${GLEW_SOURCE_DIR}/src/glew.c
  ${GLEW_SOURCE_DIR}/include
)

target_link_libraries(glew ${GLFW_LIBRARIES})

add_definitions(
  -DGLEW_STATIC
)

if (APPLE)
  if (EXISTS icon.icns)
    add_executable(${APP_NAME} MACOSX_BUNDLE ${SOURCES} icon.icns)
    set_source_files_properties("icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  else ()
    add_executable(${APP_NAME} MACOSX_BUNDLE ${SOURCES})
  endif ()

  target_link_libraries(${APP_NAME} glfw ${OPENGL_glu_LIBRARY} ${GLFW_LIBRARIES} glew)
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE "icon.icns")

  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_STRING "OpenGL boilerplate example app")
else ()
  add_executable(${APP_NAME} ${SOURCES})
  target_link_libraries(${APP_NAME} ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES} glew)
endif()

