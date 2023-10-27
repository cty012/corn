#pragma once

#include <string>
#include <tuple>
#include <corn/geometry/vec2.h>

namespace corn {
    template <std::size_t N, typename... Values>
    concept ValidRelValArgs = (sizeof...(Values) == N) && (std::same_as<float, Values> && ...);

    namespace impl::rel_val {
        struct Token;
    }

    template <std::size_t N>
    class RelativeValue {
    public:
        RelativeValue();
        static RelativeValue<N> fromString(const std::string& expression, const std::array<std::string, N>& units);

        template <typename... Values> requires ValidRelValArgs<N, Values...>
        float calc(Values... values);

    private:
        std::vector<impl::rel_val::Token> tokens;
        std::unordered_map<std::string, size_t> unitIdx;
    };
}

#include "relative_value_template.h"
