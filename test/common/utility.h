#pragma once

#include <format>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <gtest/gtest.h>

#define EXPECT_BINARY_RETURN(binary_function, val1, val2, ret_val) do { \
        binary_function(val1, val2); if (val1 != val2) return ret_val;  \
    } while (0)

#define EXPECT_EQ_RETURN(val1, val2, ret_val) EXPECT_BINARY_RETURN(EXPECT_EQ, val1, val2, ret_val)
#define EXPECT_FLOAT_EQ_RETURN(val1, val2, ret_val) EXPECT_BINARY_RETURN(EXPECT_FLOAT_EQ, val1, val2, ret_val)

#define EXPECT_EQ_MSG_RETURN(val1, val2, msg, ret_val) \
    EXPECT_EQ_RETURN(val1, val2, (printf("%s\n", msg.c_str()), ret_val))

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
            printf("Vector 1: {\n");
            for (const T& item: vec1) {
                std::string msg = std::format("  [{}]\n", item);
                printf("%s\n", msg.c_str());
            }
            printf("}\n");
            printf("Vector 2: {\n");
            for (const T& item: vec2) {
                std::string msg = std::format("  [{}]\n", item);
                printf("%s\n", msg.c_str());
            }
            printf("}\n");
        }
        return result;
    }

    template <>
    bool VectorsEqual<std::u8string>(const std::vector<std::u8string>& vec1, const std::vector<std::u8string>& vec2);

    template <typename T>
    bool MappedVectorsEqual_(const std::vector<T>& vec1, const std::vector<T>& vec2,
                             const std::function<std::string(const T&)>& func) {

        EXPECT_EQ_RETURN(vec1.size(), vec2.size(), false);
        bool result = true;
        for (size_t i = 0; i < vec1.size(); i++) {
            bool temp = func(vec1[i]) == func(vec2[i]);
            EXPECT_TRUE(temp);
            if (!temp) result = false;
        }
        EXPECT_TRUE(result);
        return result;
    }

    template <typename T>
    bool MappedVectorsEqual(const std::vector<T>& vec1, const std::vector<T>& vec2,
                            const std::function<std::string(const T&)>& func) {

        bool result = MappedVectorsEqual_(vec1, vec2, func);
        if (!result) {
            printf("Vector 1: {\n");
            for (const T& item: vec1) {
                std::string msg = std::format("  [{}]\n", func(item));
                printf("%s\n", msg.c_str());
            }
            printf("}\n");
            printf("Vector 2: {\n");
            for (const T& item: vec2) {
                std::string msg = std::format("  [{}]\n", func(item));
                printf("%s\n", msg.c_str());
            }
            printf("}\n");
        }
        return result;
    }

    template <typename T1, typename T2>
    bool UnorderedMapsEqual(const std::unordered_map<T1, T2>& map1, const std::unordered_map<T1, T2>& map2) {
        for (const auto& pair : map1) {
            auto it = map2.find(pair.first);
            // Check if key exists in map2
            EXPECT_EQ_MSG_RETURN(
                    it == map2.end(), false,
                    std::format("Key \"{}\" not found in second map.\n", pair.first), false);
            // Check if corresponding values are equal
            EXPECT_EQ_MSG_RETURN(
                    pair.second, it->second,
                    std::format(
                            "Key \"{}\" mismatch:\n"
                            "  first map: \"{}\"\n"
                            "  second map: \"{}\"\n",
                            pair.first, pair.second, it->second),
                    false);
        }

        // Optional: Check for keys in map2 that are not in map1
        for (const auto& pair : map2) {
            EXPECT_EQ_MSG_RETURN(
                    map1.find(pair.first) == map1.end(), false,
                    std::format("Key \"{}\" not found in first map.\n", pair.first), false);
        }

        return true;
    }
}
