from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.build import cross_building


class MyProjectConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    requires = [
        "sfml/2.6.1",
        # "bgfx/cci.20230216",
        # "glfw/3.4",
        # "freetype/2.13.2",
        "icu/75.1",
        "boost/1.85.0",
        "earcut/2.2.4"
    ]

    test_requires = [
        "gtest/1.15.0"
    ]

    def configure(self):
        self.options["*"].shared = False
        self.options["sfml"].shared = self.settings.os == "Macos"
        self.options["icu"].data_packaging = "static"
