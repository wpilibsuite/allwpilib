// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::DifferentialDriveKinematics> {
  static constexpr std::string_view GetTypeString() {
    return "struct:DifferentialDriveKinematics";
  }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "double track_width"; }

  static frc::DifferentialDriveKinematics Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::DifferentialDriveKinematics& value);
};

static_assert(wpi::StructSerializable<frc::DifferentialDriveKinematics>);
