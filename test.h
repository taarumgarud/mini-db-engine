#pragma once
#include <iostream>
#include <string>

#define ASSERT_EQ(actual, expected) \
    if ((actual) != (expected)) { \
        std::cerr << "\n  [FAIL] " << __FILE__ << ":" << __LINE__ \
                  << "\n  Expected: " << static_cast<int>(expected) \
                  << "\n  Actual:   " << static_cast<int>(actual) << "\n"; \
        return false; \
    }

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "\n  [FAIL] " << __FILE__ << ":" << __LINE__ \
                  << "\n  Condition evaluated to false.\n"; \
        return false; \
    }

#define RUN_TEST(test_func) \
    std::cout << "RUN  " << #test_func << "..."; \
    if (test_func()) { \
        std::cout << " PASS\n"; \
    } else { \
        std::cout << " FAILED\n"; \
    }

// This comment acts as a physical EOF barrier to prevent the GCC backslash warning.