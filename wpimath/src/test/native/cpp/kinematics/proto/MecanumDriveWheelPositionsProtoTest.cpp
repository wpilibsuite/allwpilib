// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/MecanumDriveWheelPositions.h"

using namespace frc;

namespace {

const MecanumDriveWheelPositions kExpectedData =
    MecanumDriveWheelPositions{17.4_m, 2.29_m, 22.9_m, 1.74_m};
}  // namespace

TEST(MecanumDriveWheelPositionsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.frontLeft.value(), unpacked_data->frontLeft.value());
  EXPECT_EQ(kExpectedData.frontRight.value(),
            unpacked_data->frontRight.value());
  EXPECT_EQ(kExpectedData.rearLeft.value(), unpacked_data->rearLeft.value());
  EXPECT_EQ(kExpectedData.rearRight.value(), unpacked_data->rearRight.value());
}
