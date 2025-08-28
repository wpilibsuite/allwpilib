// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/kinematics/SwerveModuleState.hpp>
#include <wpi/struct/Struct.h>

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpi::math::SwerveModuleState> {
  static constexpr std::string_view GetTypeName() {
    return "SwerveModuleState";
  }
  static constexpr size_t GetSize() {
    return 8 + wpi::GetStructSize<wpi::math::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "double speed;Rotation2d angle";
  }

  static wpi::math::SwerveModuleState Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::SwerveModuleState& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpi::math::Rotation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<wpi::math::SwerveModuleState>);
static_assert(wpi::HasNestedStruct<wpi::math::SwerveModuleState>);
