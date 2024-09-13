// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Translation3d.h"
#include "geometry3d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Translation3d>;

const Translation3d kExpectedData = Translation3d{35.04_m, 22.9_m, 3.504_m};
}  // namespace

TEST(Translation3dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Translation3d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.X(), unpacked_data.X());
  EXPECT_EQ(kExpectedData.Y(), unpacked_data.Y());
  EXPECT_EQ(kExpectedData.Z(), unpacked_data.Z());
}
