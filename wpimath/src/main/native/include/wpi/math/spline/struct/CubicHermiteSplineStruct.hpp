// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/spline/CubicHermiteSpline.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::CubicHermiteSpline> {
  static constexpr std::string_view GetTypeName() {
    return "CubicHermiteSpline";
  }
  static constexpr size_t GetSize() { return 4 * 2 * 8; }
  static constexpr std::string_view GetSchema() {
    return "double xInitial[2];double xFinal[2];double yInitial[2];double "
           "yFinal[2]";
  }

  static wpi::math::CubicHermiteSpline Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::CubicHermiteSpline& value);
};

static_assert(wpi::util::StructSerializable<wpi::math::CubicHermiteSpline>);
