// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/linalg/struct/VectorStruct.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../StructTestBase.hpp"
#include "wpi/math/linalg/EigenCore.hpp"

using namespace wpi::math;

struct VectorStructTestData {
  using Type = Vectord<2>;

  inline static const Type kTestData{1.1, 1.2};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData == data);
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(Vector, StructTest, VectorStructTestData);
