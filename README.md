# Dependencies



# Build

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