# Corn Game Engine (Under Development)
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://github.com/cty012/corn/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-red.svg)](https://github.com/cty012/corn)

Corn is a lightweight, cross-platform PC-based game engine that supports Windows, Mac, and Linux (tested on Ubuntu).
It's designed to be easy to use and efficient, allowing developers to create high-quality games with ease.

## Features
- Compatible with Windows, macOS, and Linux
- 2D rendering (3D in future plans)

## Prerequisites
- MSVC (Windows) / MinGW-w64 13.1.0 (Windows) / Clang (macOS) / GCC (Linux)
- (Optional) [CMake](https://cmake.org/): only if building from the source
- (Optional) [vcpkg](https://vcpkg.io/): only if building from the source
- (Optional) [SFML 2.6](https://www.sfml-dev.org/): only if building from the source (installed in vcpkg)
- (Optional) [Google test](https://github.com/google/googletest): only if building the test cases (installed in vcpkg)

## Getting Started

### MSVC (Windows):
1. Clone the repo:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   ```
2. Install [SFML 2.6](https://www.sfml-dev.org/):
   ```shell
   <Path_to_your_vcpkg> install sfml:x64-windows-static
   ```
   where `<Path_to_your_vcpkg>` is your `vcpkg` installation path.
3. Build the source using Visual Studio IDE.
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<Path_to_your_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static
   cmake --build build --config Release
   ```
   where `<Path_to_your_vcpkg>` is your `vcpkg` installation path.

### MinGW-w64 (Windows):
1. Clone the repo:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   ```
2. Install [SFML 2.6](https://www.sfml-dev.org/):
   ```shell
   <Path_to_your_vcpkg> install sfml:x64-mingw-static
   ```
   where `<Path_to_your_vcpkg>` is your `vcpkg` installation path.
3. Build the source:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<Path_to_your_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static
   cmake --build build --config Release
   ```
   where `<Path_to_your_vcpkg>` is your `vcpkg` installation path.

### Clang (macOS):
1. Clone the repo:
   ```shell
   git clone https://github.com/cty012/corn.git
   cd corn
   ```
2. Install [SFML 2.6](https://www.sfml-dev.org/):
   ```shell
   <Path_to_your_vcpkg> install sfml:<Cpu_arch>-osx-release
   ```
   where `<Path_to_your_vcpkg>` is your `vcpkg` installation path and `<Cpu_arch>` is your CPU's architecture (`arm64` or `x64`).
3. Build the source:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<Path_to_your_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=arm64-osx-static
   cmake --build build --config Release
   ```
   where `<Path_to_your_vcpkg>` is your `vcpkg` installation path and `<Cpu_arch>` is your CPU's architecture (`arm64` or `x64`).

### GCC (Linux):
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
- [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng License](https://www.sfml-dev.org/license.php)
- [Google Test](https://google.github.io/googletest) - Licensed under [BSD 3-Clause License](https://github.com/google/googletest/blob/main/LICENSE)
