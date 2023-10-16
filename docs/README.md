# Corn Game Engine (Under Development)
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://github.com/cty012/corn/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-red.svg)](https://github.com/cty012/corn)

Corn is a lightweight, cross-platform PC-based game engine that supports Windows, Mac, and Linux (tested on Ubuntu).
It's designed to be easy to use and efficient, allowing developers to create high-quality games with ease.

## Features
- 2D rendering (3D in future plans)
- Compatible with Windows, MacOS, and Linux

## Prerequisites
- C++ 20
- Visual Studio (Windows) / MinGW-w64 (Windows) / Clang (MacOS) / GCC (Linux)
- [CMake](https://cmake.org/) (only if building from the source)
- [SFML 2.6](https://www.sfml-dev.org/) (only if building from the source)

## Getting Started

### Windows (Visual Studio):
1. Clone the repo and create the `deps/` folder:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   mkdir deps
   ```
2. Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
3. Install [SFML 2.6](https://www.sfml-dev.org/) binaries in the `deps/` folder.
4. Build the source using Visual Studio IDE.

### Windows (MinGW-w64):
1. Clone the repo and create the `deps/` folder:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   mkdir deps
   ```
2. Install [MinGW-w64](https://www.mingw-w64.org/downloads/).
3. Install [SFML 2.6](https://www.sfml-dev.org/) binaries in the `deps/` folder.
4. Build the source:
   ```shell
   cmd.exe /c 'cmake -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-toolchain.cmake'
   mingw32-make -C build
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
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/clang-<ARCHITECTURE>-toolchain.cmake -DSFML_PACKAGE_NAME=<PKG_NAME>
   make -C build
   ```
   Replace `<ARCHITECTURE>` with your Mac's architecture (arm64 or x86_64), and `<PKG_NAME>` with the directory name containing your SFML binaries in `deps/`.

### Linux:
1. Clone the repo:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   ```
2. Install SFML and its dependencies to the default location using your package manager or from the [SFML website](https://www.sfml-dev.org/).
   For example, on Ubuntu:
   ```shell
   sudo apt install libsfml-dev
   ```
3. Build the source:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-toolchain.cmake
   make -C build
   ```

## Documentation
The documentation of this project is generated using [Doxygen](https://www.doxygen.nl/).
To generate the documentation, run the following command:
```shell
cd docs && doxygen
```

## Acknowledgments
- [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng license](https://www.sfml-dev.org/license.php)
