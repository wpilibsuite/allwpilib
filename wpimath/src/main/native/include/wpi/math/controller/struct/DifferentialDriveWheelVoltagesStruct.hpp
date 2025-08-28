// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/controller/DifferentialDriveWheelVoltages.hpp>
#include <wpi/struct/Struct.h>

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpi::math::DifferentialDriveWheelVoltages> {
  static constexpr std::string_view GetTypeName() {
    return "DifferentialDriveWheelVoltages";
  }
  static constexpr size_t GetSize() { return 16; }
  static constexpr std::string_view GetSchema() {
    return "double left;double right";
  }

  static math::DifferentialDriveWheelVoltages Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const math::DifferentialDriveWheelVoltages& value);
};

static_assert(
    wpi::StructSerializable<wpi::math::DifferentialDriveWheelVoltages>);
