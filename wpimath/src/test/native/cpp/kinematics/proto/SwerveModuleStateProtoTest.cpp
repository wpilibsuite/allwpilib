// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/SwerveModuleState.h"
#include "kinematics.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::SwerveModuleState>;

const SwerveModuleState kExpectedData =
    SwerveModuleState{22.9, Rotation2d{3.3}};
}  // namespace

TEST(SwerveModuleStateProtoTest, Roundtrip) {
  wpi::proto::ProtobufSwerveModuleState proto;
  ProtoType::Pack(&proto, kExpectedData);

  SwerveModuleState unpacked_data = ProtoType::Unpack(proto);
  EXPECT_EQ(kExpectedData.speed.value(), unpacked_data.speed.value());
  EXPECT_EQ(kExpectedData.angle.value(), unpacked_data.angle.value());
}
