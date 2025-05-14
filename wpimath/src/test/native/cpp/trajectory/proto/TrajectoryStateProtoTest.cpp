// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/trajectory/Trajectory.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Trajectory::State>;

const Trajectory::State EXPECTED_DATA = Trajectory::State{
    1.91_s, 4.4_mps, 17.4_mps_sq,
    Pose2d{Translation2d{1.74_m, 19.1_m}, Rotation2d{22.9_rad}},
    units::curvature_t{0.174}};
}  // namespace

TEST(TrajectoryStateProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.t.value(), unpacked_data->t.value());
  EXPECT_EQ(EXPECTED_DATA.velocity.value(), unpacked_data->velocity.value());
  EXPECT_EQ(EXPECTED_DATA.acceleration.value(),
            unpacked_data->acceleration.value());
  EXPECT_EQ(EXPECTED_DATA.pose, unpacked_data->pose);
  EXPECT_EQ(EXPECTED_DATA.curvature.value(), unpacked_data->curvature.value());
}
