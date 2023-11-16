// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"

template <>
struct wpi::Struct<frc::DifferentialDriveKinematics> {
  static constexpr std::string_view kTypeString =
      "struct:DifferentialDriveKinematics";
  static constexpr size_t kSize = 8;
  static constexpr std::string_view kSchema = "double track_width";

  static frc::DifferentialDriveKinematics Unpack(
      std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::DifferentialDriveKinematics& value);
};
