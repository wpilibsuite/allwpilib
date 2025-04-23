// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/system/plant/DCMotor.h"

using namespace frc;

using ProtoType = wpi::Protobuf<frc::DCMotor>;

inline constexpr DCMotor kExpectedData =
    DCMotor{1.91_V, 19.1_Nm, 1.74_A, 2.29_A, 2.2_rad_per_s};

TEST(DCMotorProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.nominalVoltage.value(),
            unpacked_data->nominalVoltage.value());
  EXPECT_EQ(kExpectedData.stallTorque.value(),
            unpacked_data->stallTorque.value());
  EXPECT_EQ(kExpectedData.stallCurrent.value(),
            unpacked_data->stallCurrent.value());
  EXPECT_EQ(kExpectedData.freeCurrent.value(),
            unpacked_data->freeCurrent.value());
  EXPECT_EQ(kExpectedData.freeSpeed.value(), unpacked_data->freeSpeed.value());
  EXPECT_EQ(kExpectedData.R.value(), unpacked_data->R.value());
  EXPECT_EQ(kExpectedData.Kv.value(), unpacked_data->Kv.value());
  EXPECT_EQ(kExpectedData.Kt.value(), unpacked_data->Kt.value());
}
