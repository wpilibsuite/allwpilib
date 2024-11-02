// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/SwerveModulePosition.h"

using namespace frc;

namespace {

const SwerveModulePosition kExpectedData =
    SwerveModulePosition{3.504_m, Rotation2d{17.4_rad}};
}  // namespace

TEST(SwerveModulePositionProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.distance.value(), unpacked_data->distance.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data->angle);
}
