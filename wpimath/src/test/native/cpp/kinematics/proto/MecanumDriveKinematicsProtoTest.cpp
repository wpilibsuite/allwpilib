// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/MecanumDriveKinematics.h"

using namespace frc;

namespace {

const MecanumDriveKinematics EXPECTED_DATA = MecanumDriveKinematics{
    Translation2d{19.1_m, 2.2_m}, Translation2d{35.04_m, 1.91_m},
    Translation2d{1.74_m, 3.504_m}, Translation2d{3.504_m, 1.91_m}};
}  // namespace

TEST(MecanumDriveKinematicsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.GetFrontLeft(), unpacked_data->GetFrontLeft());
  EXPECT_EQ(EXPECTED_DATA.GetFrontRight(), unpacked_data->GetFrontRight());
  EXPECT_EQ(EXPECTED_DATA.GetRearLeft(), unpacked_data->GetRearLeft());
  EXPECT_EQ(EXPECTED_DATA.GetRearRight(), unpacked_data->GetRearRight());
}
