// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/rotated_span.h"  // NOLINT(build/include_order)

#include <array>
#include <vector>

#include <gtest/gtest.h>

// constexpr
static constexpr std::array<int, 10> cesarr_values = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
static constexpr int cearr_values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// const
static const std::array<int, 10> csarr_values = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
static const int carr_values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static const std::vector<int> cvec_values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// non-const
static std::array<int, 10> sarr_values = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
static int arr_values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static std::vector<int> vec_values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

TEST(CircularSpanTest, Constexpr) {
  {
    constexpr wpi::rotated_span<const int, 10> sp{cesarr_values};
    static_assert(sp[5] == cesarr_values[5]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cearr_values};
    static_assert(sp[5] == cearr_values[5]);
  }
}

TEST(CircularSpanTest, ConstructConst) {
  {
    wpi::rotated_span<const int, 10> sp{csarr_values};
    EXPECT_EQ(sp[5], sarr_values[5]);
  }
  {
    wpi::rotated_span<const int> sp{csarr_values};
    EXPECT_EQ(sp[5], sarr_values[5]);
  }
  {
    wpi::rotated_span<const int, 10> sp{carr_values};
    EXPECT_EQ(sp[5], arr_values[5]);
  }
  {
    wpi::rotated_span<const int> sp{carr_values};
    EXPECT_EQ(sp[5], arr_values[5]);
  }
  {
    wpi::rotated_span<const int> sp{cvec_values.begin(), cvec_values.end()};
    EXPECT_EQ(sp[5], vec_values[5]);
  }
  {
    wpi::rotated_span<const int> sp{cvec_values.data(), cvec_values.size()};
    EXPECT_EQ(sp[5], vec_values[5]);
  }
}

TEST(CircularSpanTest, ConstructNonConst) {
  {
    wpi::rotated_span<int, 10> sp{sarr_values};
    EXPECT_EQ(sp[5], sarr_values[5]);
  }
  {
    wpi::rotated_span<int> sp{sarr_values};
    EXPECT_EQ(sp[5], sarr_values[5]);
  }
  {
    wpi::rotated_span<int, 10> sp{arr_values};
    EXPECT_EQ(sp[5], arr_values[5]);
  }
  {
    wpi::rotated_span<int> sp{arr_values};
    EXPECT_EQ(sp[5], arr_values[5]);
  }
  {
    wpi::rotated_span<int> sp{vec_values.begin(), vec_values.end()};
    EXPECT_EQ(sp[5], vec_values[5]);
  }
  {
    wpi::rotated_span<int> sp{vec_values.data(), vec_values.size()};
    EXPECT_EQ(sp[5], vec_values[5]);
  }
}

TEST(CircularSpanTest, ConstructRotated) {
  {
    constexpr wpi::rotated_span<const int, 10> sp{cesarr_values, 1};
    static_assert(sp[5] == cesarr_values[6]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cesarr_values, 9};
    static_assert(sp[5] == cesarr_values[4]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cesarr_values, 10};
    static_assert(sp[5] == cesarr_values[5]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cesarr_values, 11};
    static_assert(sp[5] == cesarr_values[6]);
  }

  {
    constexpr wpi::rotated_span<const int, 10> sp{cearr_values, -1};
    static_assert(sp[5] == cearr_values[4]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cearr_values, -9};
    static_assert(sp[5] == cearr_values[6]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cearr_values, -10};
    static_assert(sp[5] == cearr_values[5]);
  }
  {
    constexpr wpi::rotated_span<const int, 10> sp{cearr_values, -11};
    static_assert(sp[5] == cearr_values[4]);
  }
}

TEST(CircularSpanTest, Rotate) {
  constexpr wpi::rotated_span<const int, 10> sp{cesarr_values, 1};
  static_assert(sp[5] == cesarr_values[6]);
  static_assert(sp.rotate(2)[5] == cesarr_values[8]);
  static_assert(sp.rotate(9)[5] == cesarr_values[5]);
  static_assert(sp.rotate(10)[5] == cesarr_values[6]);
  static_assert(sp.rotate(11)[5] == cesarr_values[7]);
  static_assert(sp.rotate(-1)[5] == cesarr_values[5]);
  static_assert(sp.rotate(-2)[5] == cesarr_values[4]);
  static_assert(sp.rotate(-9)[5] == cesarr_values[7]);
  static_assert(sp.rotate(-10)[5] == cesarr_values[6]);
  static_assert(sp.rotate(-11)[5] == cesarr_values[5]);
}

void unsized_func(wpi::rotated_span<int>) {}
void const_unsized_func(wpi::rotated_span<const int>) {}
void sized_func(wpi::rotated_span<int, 10>) {}
void const_sized_func(wpi::rotated_span<const int, 10>) {}

TEST(CircularSpanTest, Implicit) {
  // unsized_func(csarr_values);  // error
  const_unsized_func(csarr_values);
  // sized_func(csarr_values);  // error
  const_sized_func(sarr_values);

  // unsized_func(carr_values);  // error
  const_unsized_func(carr_values);
  // sized_func(carr_values);  // error
  const_sized_func(carr_values);

  unsized_func(sarr_values);
  const_unsized_func(sarr_values);
  sized_func(sarr_values);
  const_sized_func(sarr_values);

  unsized_func(arr_values);
  const_unsized_func(arr_values);
  sized_func(arr_values);
  const_sized_func(arr_values);
}

TEST(CircularSpanTest, IteratorConst) {
  wpi::rotated_span<const int> sp_sarr{csarr_values};

  // iterator
  int i = 0;
  for (auto& elem : sp_sarr) {
    EXPECT_EQ(sarr_values[i], elem);
    ++i;
  }

  // const_iterator
  i = 0;
  for (const auto& elem : sp_sarr) {
    EXPECT_EQ(sarr_values[i], elem);
    ++i;
  }
}

TEST(CircularSpanTest, IteratorNonConst) {
  wpi::rotated_span<int> sp_sarr{sarr_values};

  // iterator
  int i = 0;
  for (auto& elem : sp_sarr) {
    EXPECT_EQ(sarr_values[i], elem);
    ++i;
  }

  // const_iterator
  i = 0;
  for (const auto& elem : sp_sarr) {
    EXPECT_EQ(sarr_values[i], elem);
    ++i;
  }
}
