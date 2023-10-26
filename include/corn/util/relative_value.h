#pragma once

#include <string>
#include <tuple>
#include "corn/geometry/vec2.h"

namespace corn {
    template <std::size_t N, typename... Values>
    concept ValidRVArgs = (sizeof...(Values) == N) && (std::same_as<float, Values> && ...);

    template <std::size_t N>
    class RelativeValue {
    public:
        RelativeValue();
        static RelativeValue fromString(const std::string& expression, const std::array<float, N>& units);

        template <typename... Values> requires ValidRVArgs<N, Values...>
        float calc(Values... values);

    private:
        std::array<float, N> params;
    };
}

#include "relative_value_template.cpp"
