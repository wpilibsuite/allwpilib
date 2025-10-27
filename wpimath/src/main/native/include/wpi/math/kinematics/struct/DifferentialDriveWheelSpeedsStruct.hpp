// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/DifferentialDriveWheelSpeeds.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::DifferentialDriveWheelSpeeds> {
  static constexpr std::string_view GetTypeName() {
    return "DifferentialDriveWheelSpeeds";
  }
  static constexpr size_t GetSize() { return 16; }
  static constexpr std::string_view GetSchema() {
    return "double left;double right";
  }

  static frc::DifferentialDriveWheelSpeeds Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::DifferentialDriveWheelSpeeds& value);
};

static_assert(wpi::StructSerializable<frc::DifferentialDriveWheelSpeeds>);
