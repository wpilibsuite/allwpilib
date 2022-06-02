// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "gtest/gtest.h"
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
  constexpr wpi::array<int, 3> arr1{1, 2, 3};
  static_cast<void>(arr1);

  // Test deduction guide
  constexpr wpi::array arr2{1, 2, 3};
  static_cast<void>(arr2);
}

TEST(ArrayTest, MoveOnlyTypeCompiles) {
  constexpr wpi::array<MoveOnlyType, 3> arr1{MoveOnlyType{}, MoveOnlyType{},
                                             MoveOnlyType{}};
  static_cast<void>(arr1);

  // Test deduction guide
  constexpr wpi::array arr2{MoveOnlyType{}, MoveOnlyType{}, MoveOnlyType{}};
  static_cast<void>(arr2);
}
