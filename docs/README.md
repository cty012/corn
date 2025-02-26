<img src="../logo.png" width="200" style="margin: 10px;">

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
- (Optional) [CMake](https://cmake.org/): Only if building from the source. Make sure it is in PATH.
- (Optional) [vcpkg](https://vcpkg.io/): Only if building from the source. Make sure it is in PATH.
- (Optional) [Simple and Fast Multimedia Library (SFML) 2.6](https://www.sfml-dev.org/): Only if building from the source (installed in vcpkg).
- (Optional) [International Components for Unicode (ICU)](https://icu.unicode.org/): Only if building from the source (installed in vcpkg).
- (Optional) [Earcut](https://github.com/mapbox/earcut.hpp): Only if building from the source (installed in vcpkg).
- (Optional) [GoogleTest](https://github.com/google/googletest): Only if building the test cases (installed in vcpkg).

## Getting Started

To begin, clone the repository to your local machine using:
```shell
git clone https://github.com/cty012/corn.git
cd corn
```

### Prerequisites
Ensure you have `CMake` and `vcpkg` installed and added to your PATH.
Replace `<path_to_vcpkg>` with the full path where vcpkg is installed on your system.

### Build Instructions

1. **Common step for all platforms:**

   Start by running the following command, replacing `<triplet>` with the appropriate target triplet for your platform (explained below):

    ```shell
    cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet>
    cmake --build build --config Release
    ```

2. **Platform-specific instructions:**
   - **MSVC (Windows):**

     Use the triplet `x64-windows-static-md`.

   - **MinGW-w64 (Windows):**
     > **Warning:** ICU might not compile using MinGW. MSVC is recommended on Windows systems.

     Use the triplet `x64-mingw-static`.
   - **Clang (macOS):**

     First, install the following dependencies before running cmake:
     ```shell
     brew install autoconf autoconf-archive automake libtool pkg-config
     ```

     Then, determine your Mac's CPU architecture. You can use uname -m in the terminal. It will return arm64 for Apple Silicon (like M1/M2 chips) or x86_64 for Intel-based Macs.
     If your CPU architecture is Apple Silicon then use `arm64-osx-release` as the triplet. If it is Intel then use `x64-osx-release`.

   - **GCC (Linux):** Use the triplet `x64-linux-release`.

After building, the compiled dynamic library will be available in the `build` directory.

## Installation
Install the library by running the following command in the project root:
```shell
cmake --install build
```
This will install the library into the `dist` folder.
For both debug and release builds, repeat the command with the respective build folders.

## Documentation
The documentation of this project is generated using [Doxygen](https://www.doxygen.nl/).
To generate the documentation, install Doxygen and run the following command:
```shell
cd docs && doxygen
```

## Acknowledgments
- [CMake](https://cmake.org/) - Licensed under [BSD 3-Clause License](https://cmake.org/licensing/)
- [vcpkg](https://vcpkg.io/) - Licensed under [MIT License](https://github.com/microsoft/vcpkg/blob/master/LICENSE.txt)
- [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng License](https://www.sfml-dev.org/license.php)
- [ICU](https://icu.unicode.org/) - Licensed under [ICU License](https://www.unicode.org/copyright.html#License)
- [Earcut](https://github.com/mapbox/earcut.hpp) - Licensed under [ISC License](https://github.com/mapbox/earcut.hpp/blob/master/LICENSE)
- [GoogleTest](https://google.github.io/googletest) - Licensed under [BSD 3-Clause License](https://github.com/google/googletest/blob/main/LICENSE)
