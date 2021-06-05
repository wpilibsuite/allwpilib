
#include "wpi/span.h"

#include "gtest/gtest.h"

#include <vector>

using wpi::span;

namespace {

template <typename R1, typename R2>
constexpr bool equal(R1&& r1, R2&& r2)
{
    auto first1 = std::begin(r1);
    const auto last1 = std::end(r1);
    auto first2 = std::begin(r2);
    const auto last2 = std::end(r2);

    while (first1 != last1 && first2 != last2) {
        if (*first1 != *first2) {
            return false;
        }
        ++first1;
        ++first2;
    }

    return true;
}

constexpr bool test_raw_array()
{
    int arr[] = {1, 2, 3};
    auto s = span{arr};
    static_assert(std::is_same_v<decltype(s), span<int, 3>>);

    return equal(arr, s);
}

constexpr bool test_const_raw_array()
{
    constexpr int arr[] = {1, 2, 3};
    auto s = span{arr};
    static_assert(std::is_same_v<decltype(s), span<const int, 3>>);

    return equal(arr, s);
}

constexpr bool test_std_array()
{
    auto arr = std::array<int, 3>{1, 2, 3};
    auto s = span{arr};
    static_assert(std::is_same_v<decltype(s), span<int, 3>>);

    return equal(arr, s);
}

constexpr bool test_const_std_array()
{
    const auto arr = std::array<int, 3>{1, 2, 3};
    auto s = span{arr};
    static_assert(std::is_same_v<decltype(s), span<const int, 3>>);

    return equal(arr, s);
}

bool test_vec()
{
    auto arr = std::vector{1, 2, 3};
    auto s = span{arr};
    static_assert(std::is_same_v<decltype(s), span<int>>);

    return equal(arr, s);
}

bool test_const_vec()
{
    const auto arr = std::vector{1, 2, 3};
    auto s = span{arr};
    static_assert(std::is_same_v<decltype(s), span<const int>>);

    return equal(arr, s);
}

}

TEST(Deduction, FromRawArrays)
{
    static_assert(test_raw_array());
    static_assert(test_const_raw_array());
    static_assert(test_std_array());
    static_assert(test_const_std_array());

    ASSERT_TRUE(test_std_array());
    ASSERT_TRUE(test_const_std_array());
    ASSERT_TRUE(test_vec());
    ASSERT_TRUE(test_const_vec());
}
