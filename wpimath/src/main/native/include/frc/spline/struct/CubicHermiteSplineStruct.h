// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/spline/CubicHermiteSpline.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::CubicHermiteSpline> {
  static constexpr std::string_view GetTypeName() {
    return "CubicHermiteSpline";
  }
  static constexpr size_t GetSize() { return 4 * 2 * 8; }
  static constexpr std::string_view GetSchema() {
    return "double xInitial[2];double xFinal[2];double yInitial[2];double "
           "yFinal[2]";
  }

  static frc::CubicHermiteSpline Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::CubicHermiteSpline& value);
};

static_assert(wpi::StructSerializable<frc::CubicHermiteSpline>);
