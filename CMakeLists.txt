cmake_minimum_required(VERSION 3.0)
project(GraphicsEngine)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
    src/NormalShader.cpp
    src/Texture.cpp
    src/FrameBuffer.cpp)

find_package(OpenGL REQUIRED)

# Compiler macro definitions
add_definitions(-DLOG_FILE="log.txt"
                -DSHADER_PATH="../src/shaders/"
                -DMODEL_PATH="../media/"
                -DTHROW_CONTRACT)

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

add_executable(${APP_NAME} ${SOURCES})

if (APPLE)
  set(OSX_APP_NAME GraphicsApp)
  if (EXISTS icon.icns)
    add_executable(${OSX_APP_NAME} MACOSX_BUNDLE ${SOURCES} icon.icns)
    set_source_files_properties("icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  else ()
    add_executable(${OSX_APP_NAME} MACOSX_BUNDLE ${SOURCES})
  endif ()

  target_link_libraries(${APP_NAME} glfw "-framework OpenGL" ${GLFW_LIBRARIES} glew)
  target_link_libraries(${OSX_APP_NAME} glfw "-framework OpenGL" ${GLFW_LIBRARIES} glew)
  set_target_properties(${OSX_APP_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE "icon.icns")

  set_target_properties(${OSX_APP_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})
  set_target_properties(${OSX_APP_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
  set_target_properties(${OSX_APP_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
  set_target_properties(${OSX_APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_STRING "OpenGL boilerplate example app")
else ()
  target_link_libraries(${APP_NAME} ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES} glew)
endif()

