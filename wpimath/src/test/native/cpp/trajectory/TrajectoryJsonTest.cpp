// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/TrajectoryConfig.h"
#include "frc/trajectory/TrajectoryUtil.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"

using namespace frc;

TEST(TrajectoryJsonTest, DeserializeMatches) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto trajectory = TestTrajectory::GetTrajectory(config);

  Trajectory deserialized;
  EXPECT_NO_THROW(deserialized = TrajectoryUtil::DeserializeTrajectory(
                      TrajectoryUtil::SerializeTrajectory(trajectory)));
  EXPECT_EQ(trajectory.States(), deserialized.States());
}
