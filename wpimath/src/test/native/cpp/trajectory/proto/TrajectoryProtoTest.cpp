// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/trajectory/Trajectory.h"
#include "trajectory.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Trajectory>;

const Trajectory kExpectedData = Trajectory{std::vector<frc::Trajectory::State>{
    Trajectory::State{1.1_s, 2.2_mps, 3.3_mps_sq,
                      Pose2d(Translation2d(1.1_m, 2.2_m), Rotation2d(2.2_rad)),
                      units::curvature_t{6.6}},
    Trajectory::State{2.1_s, 2.2_mps, 3.3_mps_sq,
                      Pose2d(Translation2d(2.1_m, 2.2_m), Rotation2d(2.2_rad)),
                      units::curvature_t{6.6}},
    Trajectory::State{3.1_s, 2.2_mps, 3.3_mps_sq,
                      Pose2d(Translation2d(3.1_m, 2.2_m), Rotation2d(2.2_rad)),
                      units::curvature_t{6.6}}}};
}  // namespace

TEST(TrajectoryProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  Trajectory unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.States(), unpacked_data.States());
}
