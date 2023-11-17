// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Translation3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Translation3d> {
  static constexpr std::string_view kTypeString = "struct:Translation3d";
  static constexpr size_t kSize = 24;
  static constexpr std::string_view kSchema = "double x;double y;double z";

  static frc::Translation3d Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::Translation3d& value);
};
