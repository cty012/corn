#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#define EXPECT_BINARY_RETURN(binary_function, val1, val2, ret_val) do { \
        binary_function(val1, val2); if (val1 != val2) return ret_val;  \
    } while (0)

#define EXPECT_EQ_RETURN(val1, val2, ret_val) EXPECT_BINARY_RETURN(EXPECT_EQ, val1, val2, ret_val)
#define EXPECT_FLOAT_EQ_RETURN(val1, val2, ret_val) EXPECT_BINARY_RETURN(EXPECT_FLOAT_EQ, val1, val2, ret_val)

namespace corn::test {
    template <typename T>
    bool VectorsEqual_(const std::vector<T>& vec1, const std::vector<T>& vec2) {
        EXPECT_EQ_RETURN(vec1.size(), vec2.size(), false);
        bool result = true;
        for (size_t i = 0; i < vec1.size(); i++) {
            bool temp = vec1[i] == vec2[i];
            EXPECT_TRUE(temp);
            if (!temp) result = false;
        }
        EXPECT_TRUE(result);
        return result;
    }

    template <typename T>
    bool VectorsEqual(const std::vector<T>& vec1, const std::vector<T>& vec2) {
        bool result = VectorsEqual_(vec1, vec2);
        if (!result) {
            std::cout << "  Vector 1: {" << std::endl;
            for (const T& item: vec1) {
                std::cout << "    [" << item << "]" << std::endl;
            }
            std::cout << std::endl << "  }" << std::endl;
            std::cout << "  Vector 2: {" << std::endl << "    ";
            for (const T& item: vec2) {
                std::cout << "    [" << item << "]" << std::endl;
            }
            std::cout << std::endl << "  }" << std::endl;
        }
        return result;
    }

    template <>
    bool VectorsEqual<std::u8string>(const std::vector<std::u8string>& vec1, const std::vector<std::u8string>& vec2);
}
