// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/ChassisSpeeds.h"

using namespace frc;

namespace {

const ChassisSpeeds EXPECTED_DATA =
    ChassisSpeeds{2.29_mps, 2.2_mps, 0.3504_rad_per_s};
}  // namespace

TEST(ChassisSpeedsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.vx.value(), unpacked_data->vx.value());
  EXPECT_EQ(EXPECTED_DATA.vy.value(), unpacked_data->vy.value());
  EXPECT_EQ(EXPECTED_DATA.omega.value(), unpacked_data->omega.value());
}
