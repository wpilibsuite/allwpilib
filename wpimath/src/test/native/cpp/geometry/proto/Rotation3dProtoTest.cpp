// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Rotation3d.h"
#include "geometry3d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Rotation3d>;

const Rotation3d kExpectedData =
    Rotation3d{Quaternion{2.29, 0.191, 0.191, 17.4}};
}  // namespace

TEST(Rotation3dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Rotation3d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.GetQuaternion(), unpacked_data.GetQuaternion());
}
