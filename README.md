# Setup
Requires: CMake, OpenGL

This repository includes its dependencies `GLFW3`, `GLM`, and `GLEW` as submodules that are built with project.

If you've already cloned the repository you'll need to initialize the submodules with
```
git submodule init
git submodule udpate
```
or you can clone and init with
```
git clone --recursive <REPO_URL>
```

After this is done you'll need to setup `GLEW`
```
cd external/glew
make extensions
```
and now you should be ready to build.

# Build

## Options
There are currently some hard coded paths that get built into the executable that can be set in the `CMakeLists.txt`.

Find the part of the CMakeLists with:
```
# Compiler macro definitions
```
The following line has the hard coded paths that will be built into the program

## Debug
Debug builds include compiler symbols for use by a debugger and include 'INFO' level log messages. From the root of the project directory:
```
mkdir debug
cd debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

## Release
Release builds include full compiler optimziations and only include 'ERROR' level log messages. From the root of the project directory:
```
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```