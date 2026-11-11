// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "TrajectoryOpMode.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/DifferentialTrajectory.hpp"
#include "wpi/system/Filesystem.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/json.hpp"

namespace {

/**
 * Loads a trajectory from a JSON file in the deploy directory's "trajectories"
 * folder.
 */
wpi::math::DifferentialTrajectory LoadTrajectory(const std::string& fileName) {
  const std::string path =
      wpi::filesystem::GetDeployDirectory() + "/trajectories/" + fileName;
  auto buffer = wpi::util::MemoryBuffer::GetFile(path);
  if (!buffer) {
    throw std::runtime_error{"Could not read trajectory file " + path + ": " +
                             buffer.error().message()};
  }
  const auto chars = (*buffer)->GetCharBuffer();

  const wpi::util::json json = wpi::util::json::parse_or_throw(
      std::string_view{chars.data(), chars.size()});
  return json.get<wpi::math::DifferentialTrajectory>();
}

}  // namespace

Robot::Robot() {
  // The generic AddOpMode<T>() overloads can only pass the robot to an
  // opmode's constructor. To give each opmode extra constructor arguments,
  // register a factory that captures them instead.
  AddOpModeFactory(wpi::RobotMode::AUTONOMOUS, "Straight Line", "Trajectories",
                   "Drives straight forward for 2 meters",
                   [this, trajectory = LoadTrajectory("straight.json")] {
                     return std::make_unique<TrajectoryOpMode>(*this,
                                                               trajectory);
                   });

  AddOpModeFactory(wpi::RobotMode::AUTONOMOUS, "Arc", "Trajectories",
                   "Drives along a left-curving arc",
                   [this, trajectory = LoadTrajectory("arc.json")] {
                     return std::make_unique<TrajectoryOpMode>(*this,
                                                               trajectory);
                   });

  PublishOpModes();
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
