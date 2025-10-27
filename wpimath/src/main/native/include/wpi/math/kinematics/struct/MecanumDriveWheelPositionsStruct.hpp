// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/MecanumDriveWheelPositions.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::MecanumDriveWheelPositions> {
  static constexpr std::string_view GetTypeName() {
    return "MecanumDriveWheelPositions";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double front_left;double front_right;double rear_left;double "
           "rear_right";
  }

  static frc::MecanumDriveWheelPositions Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::MecanumDriveWheelPositions& value);
};

static_assert(wpi::StructSerializable<frc::MecanumDriveWheelPositions>);
