// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/struct/SwerveDriveKinematicsStruct.h"

template <size_t NumModules>
frc::SwerveDriveKinematics<NumModules>
wpi::Struct<frc::SwerveDriveKinematics<NumModules>>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kModulesOff = 0;
  return frc::SwerveDriveKinematics<NumModules>{
      wpi::UnpackStructArray<frc::Translation2d, kModulesOff, NumModules>(
          data)};
}

template <size_t NumModules>
void wpi::Struct<frc::SwerveDriveKinematics<NumModules>>::Pack(
    std::span<uint8_t> data,
    const frc::SwerveDriveKinematics<NumModules>& value) {
  constexpr size_t kModulesOff = 0;
  wpi::PackStructArray<kModulesOff, NumModules>(data, value.GetModules());
}
