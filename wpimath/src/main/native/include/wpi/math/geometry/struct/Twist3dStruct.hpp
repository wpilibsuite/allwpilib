// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::Twist3d> {
  static constexpr std::string_view GetTypeName() { return "Twist3d"; }
  static constexpr size_t GetSize() { return 48; }
  static constexpr std::string_view GetSchema() {
    return "double dx;double dy;double dz;double rx;double ry;double rz";
  }

  static wpi::math::Twist3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpi::math::Twist3d& value);
};

static_assert(wpi::util::StructSerializable<wpi::math::Twist3d>);
