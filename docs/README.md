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
To get started with the project, clone the repository to your local machine using the following commands:
```shell
git clone https://github.com/cty012/corn.git
cd corn
```

The build process varies depending on your operating system and compiler.
Make sure you already have `CMake` and `vcpkg` installed on your machine.
You will need to replace placeholders with actual values for your setup.
For `<path_to_vcpkg>`, use the full path where vcpkg is installed on your system.

### MSVC (Windows):
1. Install SFML 2.6 using `vcpkg`:
   ```shell
   <path_to_vcpkg> install sfml:x64-windows-static
   ```
2. Build the project with `CMake`:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static
   cmake --build build --config Release
   ```

### MinGW-w64 (Windows):
1. Install SFML 2.6 using `vcpkg`:
   ```shell
   <path_to_vcpkg> install sfml:x64-mingw-static
   ```
2. Build the project with `CMake`:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static
   cmake --build build --config Release
   ```

### Clang (macOS):
1. Ensure `pkg-config` is installed (required by some dependencies):
   ```shell
   brew install pkg-config
   ```
2. Install SFML 2.6 using `vcpkg`:
   ```shell
   <path_to_vcpkg> install sfml:<cpu_arch>-osx-release
   ```
   Replace `<cpu_arch>` with `arm64` for Apple Silicon (M1/M2) or `x64` for Intel-based Macs.
3. Build the project with `CMake`:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<Cpu_arch>-osx-release
   cmake --build build --config Release
   ```

### GCC (Linux):
1. Install SFML 2.6 using `vcpkg`:
   ```shell
   <path_to_vcpkg> install sfml:x64-linux-release
   ```
2. Build the project with `CMake`:
   ```shell
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux-release
   cmake --build build --config Release
   ```

After building, you'll find the executables in the build directory.

## Documentation
The documentation of this project is generated using [Doxygen](https://www.doxygen.nl/).
To generate the documentation, run the following command:
```shell
cd docs && doxygen
```

## Acknowledgments
- [CMake](https://cmake.org/) - Licensed under [3-Clause BSD License](https://cmake.org/licensing/)
- [vcpkg](https://vcpkg.io/) - Licensed under [MIT License](https://github.com/microsoft/vcpkg/blob/master/LICENSE.txt)
- [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng License](https://www.sfml-dev.org/license.php)
- [Google Test](https://google.github.io/googletest) - Licensed under [BSD 3-Clause License](https://github.com/google/googletest/blob/main/LICENSE)
