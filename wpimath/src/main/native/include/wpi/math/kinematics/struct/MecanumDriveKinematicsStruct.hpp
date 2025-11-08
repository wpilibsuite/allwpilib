// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::MecanumDriveKinematics> {
  static constexpr std::string_view GetTypeName() {
    return "MecanumDriveKinematics";
  }
  static constexpr size_t GetSize() {
    return 4 * wpi::util::GetStructSize<wpi::math::Translation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation2d front_left;Translation2d front_right;Translation2d "
           "rear_left;Translation2d rear_right";
  }

  static wpi::math::MecanumDriveKinematics Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::MecanumDriveKinematics& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Translation2d>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::MecanumDriveKinematics>);
static_assert(wpi::util::HasNestedStruct<wpi::math::MecanumDriveKinematics>);
