// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Filesystem.hpp"

#include <string>

#include "wpi/opmode/RobotBase.hpp"
#include "wpi/util/fs.hpp"

std::string wpi::filesystem::GetLaunchDirectory() {
  return fs::current_path().string();
}

std::string wpi::filesystem::GetOperatingDirectory() {
  if constexpr (!RobotBase::IsSimulation()) {
    return "/home/systemcore";
  } else {
    return wpi::filesystem::GetLaunchDirectory();
  }
}

std::string wpi::filesystem::GetDeployDirectory() {
  if constexpr (!RobotBase::IsSimulation()) {
    return "/home/systemcore/deploy";
  } else {
    return (fs::current_path() / "src" / "main" / "deploy").string();
  }
}
