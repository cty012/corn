# Corn Game Engine (_still in development_)
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://github.com/cty012/corn/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-red.svg)](https://github.com/cty012/corn)

 A simple 2D game engine written in C++.

## Features

 - 2D (possibly 3D in the future) rendering
 - Support Windows and MacOS

## Requirements

 - C++ 20
 - On Windows: MinGW-w64
 - On MacOS: clang
 - [SFML](https://www.sfml-dev.org/) 2.6

## Installation

Currently only available in source code. Will provide compiled dll/dylib in the future.

### For Windows:

 1. Clone the repository:
    ```shell
    git clone https://github.com/cty012/corn.git
    ```

 2. Install MinGW-w64 on your machine. Unfortunately MSVC is not supported.

 3. Install SFML 2.6 binaries in the `deps` folder. See [SFML official website](https://www.sfml-dev.org/) for detailed instructions.

 4. Build the source code:
    ```shell
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-toolchain.cmake ..
    mingw32-make
    ```

### For MacOS:

 1. Clone the repository:
    ```shell
    git clone https://github.com/cty012/corn.git
    ```

 2. Install SFML 2.6 binaries in the `deps` folder. See [SFML official website](https://www.sfml-dev.org/) for detailed instructions.
    Choose the appropriate binary according to your Mac's architecture.

 3. Build the source code. If you are using ARM64 architecture:
    ```shell
    mkdir build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/clang-<ARCHITECTURE>-toolchain.cmake -DSFML_PACKAGE_NAME=<PKG_NAME>..
    mingw32-make
    ```
    Replace <ARCHITECTURE> with your Mac's architecture (arm64 or x86_64),
    and replace <PKG_NAME> with the name of the directory containing your SFML binaries installed in `/deps` previously.

## Documentation

 The documentation of this project is generated using [Doxygen](https://www.doxygen.nl/).
 To generate the documentation, navigate to the `docs` directory and run
 ```shell
 doxygen
 ```

## Credits

 - [SFML](https://www.sfml-dev.org/) - Licensed under [zlib/libpng license](https://www.sfml-dev.org/license.php)
