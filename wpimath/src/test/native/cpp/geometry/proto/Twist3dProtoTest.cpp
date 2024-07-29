// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Twist3d.h"
#include "geometry3d.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Twist3d>;

const Twist3d kExpectedData =
    Twist3d{1.1_m, 2.29_m, 35.04_m, 0.174_rad, 19.1_rad, 4.4_rad};
}  // namespace

TEST(Twist3dProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Twist3d unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.dx.value(), unpacked_data.dx.value());
  EXPECT_EQ(kExpectedData.dy.value(), unpacked_data.dy.value());
  EXPECT_EQ(kExpectedData.dz.value(), unpacked_data.dz.value());
  EXPECT_EQ(kExpectedData.rx.value(), unpacked_data.rx.value());
  EXPECT_EQ(kExpectedData.ry.value(), unpacked_data.ry.value());
  EXPECT_EQ(kExpectedData.rz.value(), unpacked_data.rz.value());
}
