// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Translation3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Translation3d> {
  static constexpr std::string_view GetTypeName() { return "Translation3d"; }
  static constexpr size_t GetSize() { return 24; }
  static constexpr std::string_view GetSchema() {
    return "double x;double y;double z";
  }

  static frc::Translation3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::Translation3d& value);
};

static_assert(wpi::StructSerializable<frc::Translation3d>);
