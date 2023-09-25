# CORN Game Engine (_still in development_)
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://github.com/cty012/corn/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-red.svg)](https://github.com/cty012/corn)

 A simple 2D game engine written in C++.

## Features

 - 2D (possibly 3D in the future) rendering
 - Support Windows and MacOS

## Requirements

 - C++ 20
 - SFML 2.6

## Installation

Currently only available in source code. Will provide compiled dll/dylib in the future.

### For Windows:

 1. Clone the repository:
    ```shell
    git clone https://github.com/cty012/corn.git
    ```

 2. Install MinGW on your machine. Unfortunately MSVC is not supported.

 3. Install SFML 2.6 in the `deps` folder. See [SFML official website](https://www.sfml-dev.org/) for detailed instructions.

 4. Build the source code using cmake.

### For MacOS:

 1. Clone the repository:
    ```shell
    git clone https://github.com/cty012/corn.git
    ```

 2. Install SFML 2.6 in the `deps` folder. See [SFML official website](https://www.sfml-dev.org/) for detailed instructions.
    Then copy the name of the package, go to `CMakeLists.txt` and paste it as the new `SFML_PACKAGE_NAME` variable.

 3. Build the source code using cmake.

## Documentation

 To generate the documentation, [Doxygen](https://www.doxygen.nl/) is required. Go to the `docs` folder and run
 ```shell
 doxygen
 ```

## Acknowledgments

 - [SFML](https://www.sfml-dev.org/)
 - [Doxygen](https://www.doxygen.nl/)
