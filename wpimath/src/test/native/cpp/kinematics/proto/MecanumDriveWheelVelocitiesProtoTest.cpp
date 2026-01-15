// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const MecanumDriveWheelVelocities kExpectedData =
    MecanumDriveWheelVelocities{2.29_mps, 17.4_mps, 4.4_mps, 0.229_mps};
}  // namespace

TEST(MecanumDriveWheelVelocitiesProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.frontLeft.value(), unpacked_data->frontLeft.value());
  EXPECT_EQ(kExpectedData.frontRight.value(),
            unpacked_data->frontRight.value());
  EXPECT_EQ(kExpectedData.rearLeft.value(), unpacked_data->rearLeft.value());
  EXPECT_EQ(kExpectedData.rearRight.value(), unpacked_data->rearRight.value());
}
