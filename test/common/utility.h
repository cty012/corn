#pragma once

#define EXPECT_BINARY_RETURN(binary_function, val1, val2, ret_val) do { \
        binary_function(val1, val2); if (val1 != val2) return ret_val;  \
    } while (0)

#define EXPECT_EQ_RETURN(val1, val2, ret_val) EXPECT_BINARY_RETURN(EXPECT_EQ, val1, val2, ret_val)
#define EXPECT_FLOAT_EQ_RETURN(val1, val2, ret_val) EXPECT_BINARY_RETURN(EXPECT_FLOAT_EQ, val1, val2, ret_val)

namespace corn::test {
}
