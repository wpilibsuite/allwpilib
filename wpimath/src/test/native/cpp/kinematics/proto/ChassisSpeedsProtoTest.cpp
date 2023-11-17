// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/ChassisSpeeds.h"
#include "kinematics.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::ChassisSpeeds>;

const ChassisSpeeds kExpectedData =
    ChassisSpeeds{2.29_mps, 2.2_mps, 0.3504_rad_per_s};
}  // namespace

TEST(ChassisSpeedsProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  ChassisSpeeds unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.vx.value(), unpacked_data.vx.value());
  EXPECT_EQ(kExpectedData.vy.value(), unpacked_data.vy.value());
  EXPECT_EQ(kExpectedData.omega.value(), unpacked_data.omega.value());
}
