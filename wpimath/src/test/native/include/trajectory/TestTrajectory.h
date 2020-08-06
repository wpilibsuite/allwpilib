/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "frc/trajectory/Trajectory.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {
class TestTrajectory {
 public:
  static Trajectory GetTrajectory(TrajectoryConfig& config) {
    // 2018 cross scale auto waypoints
    const Pose2d sideStart{1.54_ft, 23.23_ft, Rotation2d(180_deg)};
    const Pose2d crossScale{23.7_ft, 6.8_ft, Rotation2d(-160_deg)};

    config.SetReversed(true);

    auto vector = std::vector<Translation2d>{
        (sideStart + Transform2d{Translation2d(-13_ft, 0_ft), Rotation2d()})
            .Translation(),
        (sideStart +
         Transform2d{Translation2d(-19.5_ft, 5.0_ft), Rotation2d(-90_deg)})
            .Translation()};

    return TrajectoryGenerator::GenerateTrajectory(sideStart, vector,
                                                   crossScale, config);
  }
};

}  // namespace frc
