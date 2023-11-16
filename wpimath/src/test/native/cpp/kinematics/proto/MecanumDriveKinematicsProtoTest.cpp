// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/MecanumDriveKinematics.h"
#include "kinematics.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::MecanumDriveKinematics>;

const MecanumDriveKinematics kExpectedData = MecanumDriveKinematics{
    Translation2d{19.1, 2.2}, Translation2d{35.04, 1.91},
    Translation2d{1.74, 3.504}, Translation2d{3.504, 1.91}};
}  // namespace

TEST(MecanumDriveKinematicsProtoTest, Roundtrip) {
  wpi::proto::ProtobufMecanumDriveKinematics proto;
  ProtoType::Pack(&proto, kExpectedData);

  MecanumDriveKinematics unpacked_data = ProtoType::Unpack(proto);
  EXPECT_EQ(kExpectedData.frontLeft.value(), unpacked_data.frontLeft.value());
  EXPECT_EQ(kExpectedData.frontRight.value(), unpacked_data.frontRight.value());
  EXPECT_EQ(kExpectedData.rearLeft.value(), unpacked_data.rearLeft.value());
  EXPECT_EQ(kExpectedData.rearRight.value(), unpacked_data.rearRight.value());
}
