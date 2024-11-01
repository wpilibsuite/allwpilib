// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <google/protobuf/arena.h>
#include <gtest/gtest.h>

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::DifferentialDriveWheelSpeeds>;

const DifferentialDriveWheelSpeeds kExpectedData =
    DifferentialDriveWheelSpeeds{1.74_mps, 35.04_mps};
}  // namespace

TEST(DifferentialDriveWheelSpeedsProtoTest, Roundtrip) {
  wpi::ProtobufMessage<DifferentialDriveWheelSpeeds> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<DifferentialDriveWheelSpeeds> unpacked_data =
      message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.left.value(), unpacked_data.left.value());
  EXPECT_EQ(kExpectedData.right.value(), unpacked_data.right.value());
}
