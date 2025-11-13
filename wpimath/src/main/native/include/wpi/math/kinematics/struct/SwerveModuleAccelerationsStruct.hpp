// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/SwerveModuleAccelerations.hpp"

template <>
struct WPILIB_DLLEXPORT
    wpi::util::Struct<wpi::math::SwerveModuleAccelerations> {
  static constexpr std::string_view GetTypeName() {
    return "SwerveModuleAccelerations";
  }
  static constexpr size_t GetSize() {
    return 8 + wpi::util::GetStructSize<wpi::math::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "double acceleration;Rotation2d angle";
  }

  static wpi::math::SwerveModuleAccelerations Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::SwerveModuleAccelerations& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Rotation2d>(fn);
  }
};

static_assert(
    wpi::util::StructSerializable<wpi::math::SwerveModuleAccelerations>);
static_assert(wpi::util::HasNestedStruct<wpi::math::SwerveModuleAccelerations>);
