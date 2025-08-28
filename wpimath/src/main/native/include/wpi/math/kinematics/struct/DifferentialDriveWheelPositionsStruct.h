// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "wpi/math/kinematics/DifferentialDriveWheelPositions.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpi::math::DifferentialDriveWheelPositions> {
  static constexpr std::string_view GetTypeName() {
    return "DifferentialDriveWheelPositions";
  }
  static constexpr size_t GetSize() { return 16; }
  static constexpr std::string_view GetSchema() {
    return "double left;double right";
  }

  static wpi::math::DifferentialDriveWheelPositions Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::DifferentialDriveWheelPositions& value);
};

static_assert(
    wpi::StructSerializable<wpi::math::DifferentialDriveWheelPositions>);
