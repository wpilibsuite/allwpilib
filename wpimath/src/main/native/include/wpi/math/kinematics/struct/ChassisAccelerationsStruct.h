// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.h>
#include <wpi/util/struct/Struct.h>

#include "wpi/math/kinematics/ChassisAccelerations.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::ChassisAccelerations> {
  static constexpr std::string_view GetTypeName() {
    return "ChassisAccelerations";
  }
  static constexpr size_t GetSize() { return 24; }
  static constexpr std::string_view GetSchema() {
    return "double ax;double ay;double alpha";
  }

  static frc::ChassisAccelerations Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::ChassisAccelerations& value);
};

static_assert(wpi::StructSerializable<frc::ChassisAccelerations>);
