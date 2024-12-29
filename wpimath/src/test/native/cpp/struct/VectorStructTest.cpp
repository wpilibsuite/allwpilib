// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../StructTestBase.h"
#include "frc/EigenCore.h"
#include "frc/struct/VectorStruct.h"

using namespace frc;

struct VectorStructTestData {
  using Type = Vectord<2>;

  inline static const Type kTestData{1.1, 1.2};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData, data);
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(Vector, StructTest, VectorStructTestData);
