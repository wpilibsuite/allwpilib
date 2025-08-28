// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "wpimath/kinematics/SwerveModulePosition.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpimath::SwerveModulePosition> {
  static constexpr std::string_view GetTypeName() {
    return "SwerveModulePosition";
  }
  static constexpr size_t GetSize() {
    return 8 + wpi::GetStructSize<wpimath::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "double distance;Rotation2d angle";
  }

  static wpimath::SwerveModulePosition Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpimath::SwerveModulePosition& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpimath::Rotation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<wpimath::SwerveModulePosition>);
static_assert(wpi::HasNestedStruct<wpimath::SwerveModulePosition>);
