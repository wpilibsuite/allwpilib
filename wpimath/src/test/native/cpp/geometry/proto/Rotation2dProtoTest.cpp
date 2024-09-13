// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Rotation2d.h"
#include "geometry2d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Rotation2d>;

const Rotation2d kExpectedData = Rotation2d{1.91_rad};
}  // namespace

TEST(Rotation2dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Rotation2d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.Radians().value(), unpacked_data.Radians().value());
}
