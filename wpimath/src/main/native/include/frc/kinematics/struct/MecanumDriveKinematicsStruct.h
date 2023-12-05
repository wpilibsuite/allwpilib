// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/kinematics/MecanumDriveKinematics.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::MecanumDriveKinematics> {
  static constexpr std::string_view GetTypeString() {
    return "struct:MecanumDriveKinematics";
  }
  static constexpr size_t GetSize() {
    return 4 * wpi::GetStructSize<frc::Translation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation2d front_left;Translation2d front_right;Translation2d "
           "rear_left;Translation2d rear_right";
  }

  static frc::MecanumDriveKinematics Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::MecanumDriveKinematics& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Translation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::MecanumDriveKinematics>);
static_assert(wpi::HasNestedStruct<frc::MecanumDriveKinematics>);
