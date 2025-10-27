// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/kinematics/SwerveModuleState.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::SwerveModuleState> {
  static constexpr std::string_view GetTypeName() {
    return "SwerveModuleState";
  }
  static constexpr size_t GetSize() {
    return 8 + wpi::GetStructSize<frc::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "double speed;Rotation2d angle";
  }

  static frc::SwerveModuleState Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::SwerveModuleState& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Rotation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::SwerveModuleState>);
static_assert(wpi::HasNestedStruct<frc::SwerveModuleState>);
