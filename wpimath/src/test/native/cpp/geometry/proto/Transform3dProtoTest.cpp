// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Transform3d.h"
#include "geometry3d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Transform3d>;

const Transform3d kExpectedData =
    Transform3d{Translation3d{0.3504_m, 22.9_m, 3.504_m},
                Rotation3d{Quaternion{0.3504, 35.04, 2.29, 0.3504}}};
}  // namespace

TEST(Transform3dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Transform3d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.Translation(), unpacked_data.Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data.Rotation());
}
