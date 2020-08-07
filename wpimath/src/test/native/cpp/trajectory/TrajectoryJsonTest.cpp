/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
