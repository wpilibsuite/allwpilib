// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../StructTestBase.h"
#include "frc/EigenCore.h"
#include "frc/struct/MatrixStruct.h"

using namespace frc;

struct MatrixStructTestData {
  using Type = Matrixd<2, 3>;

  inline static const Type kTestData{{1.1, 1.2, 1.3}, {1.4, 1.5, 1.6}};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData, data);
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(Matrix, StructTest, MatrixStructTestData);
