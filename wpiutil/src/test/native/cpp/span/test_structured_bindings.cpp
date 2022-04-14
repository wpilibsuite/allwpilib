
#include "wpi/span.h"

#include "gtest/gtest.h"

using static_span_t = wpi::span<int, 3>;
using dynamic_span_t = wpi::span<int>;

static_assert(std::tuple_size_v<static_span_t> == static_span_t::extent);
static_assert(!wpi::detail::is_complete<std::tuple_size<dynamic_span_t>>::value);

TEST(StructuredBindings, Test)
{
    // C++, why you no let me do constexpr structured bindings?

    int arr[] = {1, 2, 3};

    auto& [a1, a2, a3] = arr;
    auto&& [s1, s2, s3] = wpi::span(arr);

    ASSERT_EQ(a1, s1);
    ASSERT_EQ(a2, s2);
    ASSERT_EQ(a3, s3);

    a1 = 99;
    ASSERT_EQ(s1, 99);

    s2 = 100;
    ASSERT_EQ(a2, 100);
}
