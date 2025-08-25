// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/kinematics/MecanumDriveWheelAccelerations.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::MecanumDriveWheelAccelerations> {
  static constexpr std::string_view GetTypeName() {
    return "MecanumDriveWheelAccelerations";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double front_left;double front_right;double rear_left;double "
           "rear_right";
  }

  static frc::MecanumDriveWheelAccelerations Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::MecanumDriveWheelAccelerations& value);
};

static_assert(wpi::StructSerializable<frc::MecanumDriveWheelAccelerations>);
