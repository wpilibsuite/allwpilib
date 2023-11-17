// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"
#include "kinematics.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::DifferentialDriveWheelSpeeds>;

const DifferentialDriveWheelSpeeds kExpectedData =
    DifferentialDriveWheelSpeeds{1.74_mps, 35.04_mps};
}  // namespace

TEST(DifferentialDriveWheelSpeedsProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  DifferentialDriveWheelSpeeds unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.left.value(), unpacked_data.left.value());
  EXPECT_EQ(kExpectedData.right.value(), unpacked_data.right.value());
}
