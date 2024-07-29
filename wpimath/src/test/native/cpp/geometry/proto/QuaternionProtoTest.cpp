// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Quaternion.h"
#include "geometry3d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Quaternion>;

const Quaternion kExpectedData = Quaternion{1.1, 0.191, 35.04, 19.1};
}  // namespace

TEST(QuaternionProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Quaternion unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.W(), unpacked_data.W());
  EXPECT_EQ(kExpectedData.X(), unpacked_data.X());
  EXPECT_EQ(kExpectedData.Y(), unpacked_data.Y());
  EXPECT_EQ(kExpectedData.Z(), unpacked_data.Z());
}
