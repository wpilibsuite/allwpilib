// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/MecanumDriveWheelSpeeds.h"

using namespace frc;

namespace {

const MecanumDriveWheelSpeeds EXPECTED_DATA =
    MecanumDriveWheelSpeeds{2.29_mps, 17.4_mps, 4.4_mps, 0.229_mps};
}  // namespace

TEST(MecanumDriveWheelSpeedsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.frontLeft.value(), unpacked_data->frontLeft.value());
  EXPECT_EQ(EXPECTED_DATA.frontRight.value(),
            unpacked_data->frontRight.value());
  EXPECT_EQ(EXPECTED_DATA.rearLeft.value(), unpacked_data->rearLeft.value());
  EXPECT_EQ(EXPECTED_DATA.rearRight.value(), unpacked_data->rearRight.value());
}
