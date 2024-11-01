// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <google/protobuf/arena.h>
#include <gtest/gtest.h>

#include "frc/kinematics/SwerveModuleState.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::SwerveModuleState>;

const SwerveModuleState kExpectedData =
    SwerveModuleState{22.9_mps, Rotation2d{3.3_rad}};
}  // namespace

TEST(SwerveModuleStateProtoTest, Roundtrip) {
  wpi::ProtobufMessage<SwerveModuleState> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<SwerveModuleState> unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.speed.value(), unpacked_data.speed.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data.angle);
}
