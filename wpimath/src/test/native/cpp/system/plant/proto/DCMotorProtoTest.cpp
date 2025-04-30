// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/system/plant/DCMotor.h"

using namespace frc;

using ProtoType = wpi::Protobuf<frc::DCMotor>;

inline constexpr DCMotor EXPECTED_DATA =
    DCMotor{1.91_V, 19.1_Nm, 1.74_A, 2.29_A, 2.2_rad_per_s, 2};

TEST(DCMotorProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.nominalVoltage.value(),
            unpacked_data->nominalVoltage.value());
  EXPECT_EQ(EXPECTED_DATA.stallTorque.value(),
            unpacked_data->stallTorque.value());
  EXPECT_EQ(EXPECTED_DATA.stallCurrent.value(),
            unpacked_data->stallCurrent.value());
  EXPECT_EQ(EXPECTED_DATA.freeCurrent.value(),
            unpacked_data->freeCurrent.value());
  EXPECT_EQ(EXPECTED_DATA.freeSpeed.value(), unpacked_data->freeSpeed.value());
  EXPECT_EQ(EXPECTED_DATA.R.value(), unpacked_data->R.value());
  EXPECT_EQ(EXPECTED_DATA.V.value(), unpacked_data->V.value());
  EXPECT_EQ(EXPECTED_DATA.Kt.value(), unpacked_data->Kt.value());
}
