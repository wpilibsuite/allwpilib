// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"

using namespace frc;

namespace {

const DifferentialDriveWheelSpeeds EXPECTED_DATA =
    DifferentialDriveWheelSpeeds{1.74_mps, 35.04_mps};
}  // namespace

TEST(DifferentialDriveWheelSpeedsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.left.value(), unpacked_data->left.value());
  EXPECT_EQ(EXPECTED_DATA.right.value(), unpacked_data->right.value());
}
