// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Twist3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Twist3d> {
  static constexpr std::string_view GetTypeName() { return "Twist3d"; }
  static constexpr size_t GetSize() { return 48; }
  static constexpr std::string_view GetSchema() {
    return "double dx;double dy;double dz;double rx;double ry;double rz";
  }

  static frc::Twist3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::Twist3d& value);
};

static_assert(wpi::StructSerializable<frc::Twist3d>);
