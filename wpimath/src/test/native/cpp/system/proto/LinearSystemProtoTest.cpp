// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../../ProtoTestBase.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/proto/LinearSystemProto.h"

using namespace frc;

struct LinearSystemProtoTestData {
  using Type = LinearSystem<2, 3, 4>;

  inline static const Type kTestData{
      Matrixd<2, 2>{{1.1, 1.2}, {1.3, 1.4}},
      Matrixd<2, 3>{{2.1, 2.2, 2.3}, {2.4, 2.5, 2.6}},
      Matrixd<4, 2>{{3.1, 3.2}, {3.3, 3.4}, {3.5, 3.6}, {3.7, 3.8}},
      Matrixd<4, 3>{{4.01, 4.02, 4.03},
                    {4.04, 4.05, 4.06},
                    {4.07, 4.08, 4.09},
                    {4.10, 4.11, 4.12}}};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.A(), data.A());
    EXPECT_EQ(testData.B(), data.B());
    EXPECT_EQ(testData.C(), data.C());
    EXPECT_EQ(testData.D(), data.D());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(LinearSystem, ProtoTest,
                               LinearSystemProtoTestData);
