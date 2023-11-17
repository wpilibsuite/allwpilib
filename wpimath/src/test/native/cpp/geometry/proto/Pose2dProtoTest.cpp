// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Pose2d.h"
#include "geometry2d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Pose2d>;

const Pose2d kExpectedData =
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}};
}  // namespace

TEST(Pose2dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Pose2d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.Translation(), unpacked_data.Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data.Rotation());
}
