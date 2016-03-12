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
    src/test.cpp
    src/InputHandler.cpp

    src/Logger.cpp
    src/Util.cpp
    src/Window.cpp
    
    src/SceneGraph.cpp
    src/CoordFrame.cpp
    src/FrameBuffer.cpp

    src/RenderPipeline.cpp
    src/ShadowPipeline.cpp

    src/Texture/Texture.cpp
    src/Texture/CubeMap.cpp
    src/Texture/ParsePPMTexture.cpp
    src/Texture/ParseTGATexture.cpp

    src/Mesh/Mesh.cpp
    src/Mesh/MeshSimplifier.cpp
    src/Mesh/ParseOFFMesh.cpp
    src/Mesh/ParseOBJMesh.cpp

    src/Shader/Shader.cpp
    src/Shader/FlatShader.cpp
    src/Shader/NormalShader.cpp
    src/Shader/ShadowShader.cpp
    src/Shader/LightShader.cpp)

find_package(OpenGL REQUIRED)

# Compiler macro definitions
add_definitions(-DLOG_FILE="log.txt"
                -DSHADER_PATH="../src/Shader/"
                -DMODEL_PATH="../media/model/"
                -DTEXTURE_PATH="../media/texture/"
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

