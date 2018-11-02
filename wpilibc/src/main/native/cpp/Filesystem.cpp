/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Filesystem.h"

#include <wpi/FileSystem.h>
#include <wpi/Path.h>

#include "frc/RobotBase.h"

void frc::filesystem::GetLaunchDirectory(wpi::SmallVectorImpl<char>& result) {
  wpi::sys::fs::current_path(result);
}

void frc::filesystem::GetOperatingDirectory(
    wpi::SmallVectorImpl<char>& result) {
  if (RobotBase::IsReal()) {
    wpi::sys::path::native("/home/lvuser", result);
  } else {
    frc::filesystem::GetLaunchDirectory(result);
  }
}

void frc::filesystem::GetDeployDirectory(wpi::SmallVectorImpl<char>& result) {
  frc::filesystem::GetOperatingDirectory(result);
  wpi::sys::path::append(result, "deploy");
}
