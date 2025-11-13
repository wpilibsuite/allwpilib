// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::Twist2d> {
  static constexpr std::string_view GetTypeName() { return "Twist2d"; }
  static constexpr size_t GetSize() { return 24; }
  static constexpr std::string_view GetSchema() {
    return "double dx;double dy;double dtheta";
  }

  static wpi::math::Twist2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpi::math::Twist2d& value);
};

static_assert(wpi::util::StructSerializable<wpi::math::Twist2d>);
