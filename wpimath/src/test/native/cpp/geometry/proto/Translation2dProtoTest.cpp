// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Translation2d.h"
#include "geometry2d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Translation2d>;

const Translation2d kExpectedData = Translation2d{3.504_m, 22.9_m};
}  // namespace

TEST(Translation2dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Translation2d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.X().value(), unpacked_data.X().value());
  EXPECT_EQ(kExpectedData.Y().value(), unpacked_data.Y().value());
}
