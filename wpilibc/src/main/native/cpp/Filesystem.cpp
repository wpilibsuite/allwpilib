// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Filesystem.h"

#include <wpi/FileSystem.h>
#include <wpi/Path.h>

#include "frc/RobotBase.h"

void frc::filesystem::GetLaunchDirectory(wpi::SmallVectorImpl<char>& result) {
  wpi::sys::fs::current_path(result);
}

void frc::filesystem::GetOperatingDirectory(
    wpi::SmallVectorImpl<char>& result) {
  if constexpr (RobotBase::IsReal()) {
    wpi::sys::path::native("/home/lvuser", result);
  } else {
    frc::filesystem::GetLaunchDirectory(result);
  }
}

void frc::filesystem::GetDeployDirectory(wpi::SmallVectorImpl<char>& result) {
  frc::filesystem::GetOperatingDirectory(result);
  if constexpr (RobotBase::IsReal()) {
    wpi::sys::path::append(result, "deploy");
  } else {
    wpi::sys::path::append(result, "src");
    wpi::sys::path::append(result, "main");
    wpi::sys::path::append(result, "deploy");
  }
}
