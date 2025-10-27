// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/util/SmallVector.hpp>

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

using namespace wpi::math;

namespace {

const DifferentialDriveKinematics kExpectedData =
    DifferentialDriveKinematics{1.74_m};
}  // namespace

TEST(DifferentialDriveKinematicsProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.trackwidth.value(),
            unpacked_data->trackwidth.value());
}
