// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "kinematics.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::DifferentialDriveKinematics>;

const DifferentialDriveKinematics kExpectedData =
    DifferentialDriveKinematics{1.74_m};
}  // namespace

TEST(DifferentialDriveKinematicsProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  DifferentialDriveKinematics unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.trackWidth.value(), unpacked_data.trackWidth.value());
}
