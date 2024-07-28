// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/kinematics/MecanumDriveWheelSpeeds.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::MecanumDriveWheelSpeeds> {
  static constexpr std::string_view GetTypeName() {
    return "MecanumDriveWheelSpeeds";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double front_left;double front_right;double rear_left;"
           "double rear_right";
  }

  static frc::MecanumDriveWheelSpeeds Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::MecanumDriveWheelSpeeds& value);
};

static_assert(wpi::StructSerializable<frc::MecanumDriveWheelSpeeds>);
