// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <google/protobuf/arena.h>
#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"

using namespace frc;

namespace {

const DifferentialDriveKinematics EXPECTED_DATA =
    DifferentialDriveKinematics{1.74_m};
}  // namespace

TEST(DifferentialDriveKinematicsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.trackwidth.value(),
            unpacked_data->trackwidth.value());
}
