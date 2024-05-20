// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/array.h"

namespace {
class MoveOnlyType {
 public:
  MoveOnlyType() = default;
  MoveOnlyType(MoveOnlyType&&) = default;
  MoveOnlyType& operator=(MoveOnlyType&&) = default;
};
}  // namespace

TEST(ArrayTest, CopyableTypeCompiles) {
  [[maybe_unused]]
  constexpr wpi::array<int, 3> arr1{1, 2, 3};

  // Test deduction guide
  [[maybe_unused]]
  constexpr wpi::array arr2{1, 2, 3};
}

TEST(ArrayTest, MoveOnlyTypeCompiles) {
  [[maybe_unused]]
  constexpr wpi::array<MoveOnlyType, 3> arr1{MoveOnlyType{}, MoveOnlyType{},
                                             MoveOnlyType{}};

  // Test deduction guide
  [[maybe_unused]]
  constexpr wpi::array arr2{MoveOnlyType{}, MoveOnlyType{}, MoveOnlyType{}};
}
