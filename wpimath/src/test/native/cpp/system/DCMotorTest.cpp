// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>

#include <gtest/gtest.h>

#include "frc/system/plant/DCMotor.h"

using namespace frc;

TEST(DCMotorTest, Equality) {
  const DCMotor a = DCMotor::KrakenX60(2);
  const DCMotor b{12_V, 14.18_Nm, 732_A, 4_A, 6000_rpm, 1};
  EXPECT_TRUE(a == b);
}