// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/SwerveModulePosition.h"
#include "kinematics.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::SwerveModulePosition>;

const SwerveModulePosition kExpectedData =
    SwerveModulePosition{3.504_m, Rotation2d{17.4_rad}};
}  // namespace

TEST(SwerveModulePositionProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  SwerveModulePosition unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.distance.value(), unpacked_data.distance.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data.angle);
}
