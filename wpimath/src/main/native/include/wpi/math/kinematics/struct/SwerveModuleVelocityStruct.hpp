// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::SwerveModuleVelocity> {
  static constexpr std::string_view GetTypeName() {
    return "SwerveModuleVelocity";
  }
  static constexpr size_t GetSize() {
    return 8 + wpi::util::GetStructSize<wpi::math::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "double velocity;Rotation2d angle";
  }

  static wpi::math::SwerveModuleVelocity Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::SwerveModuleVelocity& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Rotation2d>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::SwerveModuleVelocity>);
static_assert(wpi::util::HasNestedStruct<wpi::math::SwerveModuleVelocity>);
