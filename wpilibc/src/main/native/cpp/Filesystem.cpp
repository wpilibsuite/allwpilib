// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Filesystem.h"

#include <string>

#include <wpi/fs.h>

#include "frc/RobotBase.h"

std::string frc::filesystem::GetLaunchDirectory() {
  return fs::current_path().string();
}

std::string frc::filesystem::GetOperatingDirectory() {
  if constexpr (!RobotBase::IsSimulation()) {
    return "/home/lvuser";
  } else {
    return frc::filesystem::GetLaunchDirectory();
  }
}

std::string frc::filesystem::GetDeployDirectory() {
  if constexpr (!RobotBase::IsSimulation()) {
    return "/home/lvuser/deploy";
  } else {
    return (fs::current_path() / "src" / "main" / "deploy").string();
  }
}
