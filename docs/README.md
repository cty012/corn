# Corn Game Engine (Under Development)
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://github.com/cty012/corn/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-red.svg)](https://github.com/cty012/corn)

Corn is a simple 2D game engine crafted with C++.

## Features
- 2D rendering (3D in future plans)
- Compatible with Windows, MacOS, and Linux

## Prerequisites
- C++ 20
- MinGW-w64 (Windows) / clang (MacOS) / GNU (Linux)
- [CMake](https://cmake.org/) (only if building from the source)
- [SFML 2.6](https://www.sfml-dev.org/) (only if building from the source)

## Getting Started

### Windows:
1. Clone the repo and create the `deps/` folder:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   mkdir deps
   ```
2. Install [MinGW-w64](https://www.mingw-w64.org/downloads/). (MSVC not supported)
3. Install [SFML 2.6](https://www.sfml-dev.org/) binaries in the `deps/` folder.
4. Build the source:
   ```shell
   mkdir build && cd build
   cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-toolchain.cmake ..
   mingw32-make
   ```

### MacOS:
1. Clone the repo and create the `deps/` folder:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   mkdir deps
   ```
2. Install [SFML 2.6](https://www.sfml-dev.org/) binaries in the `deps/` folder.
3. Build the source:
   ```shell
   mkdir build && cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/clang-<ARCHITECTURE>-toolchain.cmake -DSFML_PACKAGE_NAME=<PKG_NAME> ..
   make
   ```
   Replace `<ARCHITECTURE>` with your Mac's architecture (arm64 or x86_64), and `<PKG_NAME>` with the directory name containing your SFML binaries in `deps/`.

### Linux:
1. Clone the repo:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   ```
2. Install [SFML 2.6](https://www.sfml-dev.org/) and its dependencies. Do not put them in the `deps/` folder. Put them in the default location instead.
3. Build the source:
   ```shell
   mkdir build && cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/gnu-toolchain.cmake ..
   make
   ```

## Documentation
The documentation of this project is generated using [Doxygen](https://www.doxygen.nl/).
To generate the documentation, run the following command:
```shell
cd docs && doxygen
```

## Acknowledgments
- [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng license](https://www.sfml-dev.org/license.php)
