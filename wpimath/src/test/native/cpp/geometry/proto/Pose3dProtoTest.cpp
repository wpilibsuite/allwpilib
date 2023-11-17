// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Pose3d.h"
#include "geometry3d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Pose3d>;

const Pose3d kExpectedData =
    Pose3d{Translation3d{1.1_m, 2.2_m, 1.1_m},
           Rotation3d{Quaternion{1.91, 0.3504, 3.3, 1.74}}};
}  // namespace

TEST(Pose3dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Pose3d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.Translation(), unpacked_data.Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data.Rotation());
}
