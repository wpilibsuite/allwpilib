// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/system/plant/DCMotor.h"

using namespace frc;

using StructType = wpi::Struct<frc::DCMotor>;

inline constexpr DCMotor kExpectedData =
    DCMotor{1.91_V, 19.1_Nm, 1.74_A, 2.29_A, 2.2_rad_per_s, 2};

TEST(DCMotorStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  DCMotor unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.nominalVoltage.value(),
            unpacked_data.nominalVoltage.value());
  EXPECT_EQ(kExpectedData.stallTorque.value(),
            unpacked_data.stallTorque.value());
  EXPECT_EQ(kExpectedData.stallCurrent.value(),
            unpacked_data.stallCurrent.value());
  EXPECT_EQ(kExpectedData.freeCurrent.value(),
            unpacked_data.freeCurrent.value());
  EXPECT_EQ(kExpectedData.freeSpeed.value(), unpacked_data.freeSpeed.value());
  EXPECT_EQ(kExpectedData.R.value(), unpacked_data.R.value());
  EXPECT_EQ(kExpectedData.Kv.value(), unpacked_data.Kv.value());
  EXPECT_EQ(kExpectedData.Kt.value(), unpacked_data.Kt.value());
}
