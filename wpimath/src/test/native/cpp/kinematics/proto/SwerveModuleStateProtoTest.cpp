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
    SwerveModuleState{22.9_mps, Rotation2d{3.3_rad}};
}  // namespace

TEST(SwerveModuleStateProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  SwerveModuleState unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.speed.value(), unpacked_data.speed.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data.angle);
}
