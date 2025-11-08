// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Filesystem.hpp"

#include <string>

#include "wpi/opmode/RobotBase.hpp"
#include "wpi/util/fs.hpp"

std::string frc::filesystem::GetLaunchDirectory() {
  return fs::current_path().string();
}

std::string frc::filesystem::GetOperatingDirectory() {
  if constexpr (!RobotBase::IsSimulation()) {
    return "/home/systemcore";
  } else {
    return frc::filesystem::GetLaunchDirectory();
  }
}

std::string frc::filesystem::GetDeployDirectory() {
  if constexpr (!RobotBase::IsSimulation()) {
    return "/home/systemcore/deploy";
  } else {
    return (fs::current_path() / "src" / "main" / "deploy").string();
  }
}
