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
- (Optional) [Conan](https://conan.io/): Only if building from the source. Make sure it is in PATH.
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
Ensure you have `CMake` and `Conan` installed and added to your PATH.
Check their respective websites for installation and setup instructions.

MacOS users may need to install the following packages required by Conan:
```shell
brew install autoconf autoconf-archive automake libtool pkg-config
```

### Build Instructions
Run the following command, replacing `<profile>` with your own Conan profile:

```shell
conan install . -pr=<profile> -of=conan/<profile> -b=missing
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan/<profile>/conan_toolchain.cmake
cmake --build build --config Release
```

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
- [CMake](https://cmake.org/) - Licensed under [3-Clause BSD License](https://cmake.org/licensing/)
- [Conan](https://conan.io/) - Licensed under [MIT License](https://github.com/conan-io/conan/blob/develop2/LICENSE.md)
- [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng License](https://www.sfml-dev.org/license.php)
- [ICU](https://icu.unicode.org/) - Licensed under [ICU License](https://www.unicode.org/copyright.html#License)
- [Earcut](https://github.com/mapbox/earcut.hpp) - Licensed under [ISC License](https://github.com/mapbox/earcut.hpp/blob/master/LICENSE)
- [GoogleTest](https://google.github.io/googletest) - Licensed under [BSD 3-Clause License](https://github.com/google/googletest/blob/main/LICENSE)
