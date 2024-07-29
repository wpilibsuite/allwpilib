// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
    const Pose2d sideStart{1.54_ft, 23.23_ft, 180_deg};
    const Pose2d crossScale{23.7_ft, 6.8_ft, -160_deg};

    config.SetReversed(true);

    auto vector = std::vector<Translation2d>{
        (sideStart + Transform2d{Translation2d{-13_ft, 0_ft}, 0_deg})
            .Translation(),
        (sideStart + Transform2d{Translation2d{-19.5_ft, 5.0_ft}, -90_deg})
            .Translation()};

    return TrajectoryGenerator::GenerateTrajectory(sideStart, vector,
                                                   crossScale, config);
  }
};

}  // namespace frc
