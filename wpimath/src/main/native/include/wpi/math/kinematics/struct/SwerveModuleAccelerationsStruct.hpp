// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/SwerveModuleAccelerations.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::SwerveModuleAccelerations> {
  static constexpr std::string_view GetTypeName() {
    return "SwerveModuleAccelerations";
  }
  static constexpr size_t GetSize() {
    return 8 + wpi::GetStructSize<frc::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "double acceleration;Rotation2d angle";
  }

  static frc::SwerveModuleAccelerations Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::SwerveModuleAccelerations& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Rotation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::SwerveModuleAccelerations>);
static_assert(wpi::HasNestedStruct<frc::SwerveModuleAccelerations>);
