/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
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
  static Trajectory GetTrajectory(
      std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints) {
    const units::meters_per_second_t startVelocity = 0_mps;
    const units::meters_per_second_t endVelocity = 0_mps;
    const units::meters_per_second_t maxVelocity = 12_fps;
    const units::meters_per_second_squared_t maxAcceleration = 12_fps_sq;

    // 2018 cross scale auto waypoints
    const Pose2d sideStart{1.54_ft, 23.23_ft, Rotation2d(180_deg)};
    const Pose2d crossScale{23.7_ft, 6.8_ft, Rotation2d(-160_deg)};

    auto vector = std::vector<Translation2d>{
        (sideStart + Transform2d{Translation2d(-13_ft, 0_ft), Rotation2d()})
            .Translation(),
        (sideStart +
         Transform2d{Translation2d(-19.5_ft, 5.0_ft), Rotation2d(-90_deg)})
            .Translation()};

    return TrajectoryGenerator::GenerateTrajectory(
        sideStart, vector, crossScale, std::move(constraints), startVelocity,
        endVelocity, maxVelocity, maxAcceleration, true);
  }
};

}  // namespace frc
