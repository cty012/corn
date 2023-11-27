/**
 * @file constants.h
 * @brief Defines common constants.
 *
 * Some compilers, such as GCC and Clang, provide built-in values of math constants. Others like MSVC do not provide
 * them. Therefore, common constants are defined here to ensure consistent performance across different platforms and
 * compilers.
 */

#pragma once

namespace corn {
	constexpr double E = 2.7182818284590452354;
	constexpr double PI = 3.14159265358979323846;
}
