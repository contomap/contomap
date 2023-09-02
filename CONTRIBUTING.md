# Contributing to Contomap

This project is at an early stage of being a hobby-project.

When contributing to this repository, please first discuss the change you wish to make
first via an issue before making a change.

## Building

### Dependencies

* CMake - install as per https://cmake.org/ (See `CMakeLists.txt` for minimum required version)
* MinGW - capable of C++20
* For web (WASM)
    * emscripten - install as per https://emscripten.org/ (Which requires Python, indirectly)

### On command-line

The full build is automated with the `build.sh` script.
Run it on a Linux shell without arguments to see all the possibilities.

Manual steps described below (although, barely maintained).

#### On Linux

##### For MS Windows

```
mkdir cmake-build-debug-mingw-cross
cd cmake-build-debug-mingw-cross
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake ..
make -j 6
```

##### For WASM

```
mkdir cmake-build-debug-emscripten-cross
cd cmake-build-debug-emscripten-cross
cmake -DPLATFORM=Web -DCMAKE_TOOLCHAIN_FILE=<path-to-emsdk>/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ..
make -j 6
```

Note: Run a web server with `python3 -m http.server 8080`, then open `http://localhost:8080/contomap-wasm.html` with the
browser.

### Further resources

* Raylib
    * https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)
    * https://stackoverflow.com/questions/51868832/integrate-emscripten-in-clion
