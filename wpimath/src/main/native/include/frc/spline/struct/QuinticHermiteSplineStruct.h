// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/spline/QuinticHermiteSpline.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::QuinticHermiteSpline> {
  static constexpr std::string_view kTypeString = "struct:QuinticHermiteSpline";
  static constexpr size_t kSize = 4 * 3 * 8;
  static constexpr std::string_view kSchema =
      "double xInitial[3];double xFinal[3];double yInitial[3];double yFinal[3]";

  static frc::QuinticHermiteSpline Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::QuinticHermiteSpline& value);
};
