// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Twist2d.h"
#include "geometry2d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Twist2d>;

const Twist2d kExpectedData = Twist2d{2.29_m, 35.04_m, 35.04_rad};
}  // namespace

TEST(Twist2dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Twist2d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.dx.value(), unpacked_data.dx.value());
  EXPECT_EQ(kExpectedData.dy.value(), unpacked_data.dy.value());
  EXPECT_EQ(kExpectedData.dtheta.value(), unpacked_data.dtheta.value());
}
