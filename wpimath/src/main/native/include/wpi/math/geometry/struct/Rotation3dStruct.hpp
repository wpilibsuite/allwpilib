// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/geometry/Rotation3d.hpp>
#include <wpi/struct/Struct.h>

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpi::math::Rotation3d> {
  static constexpr std::string_view GetTypeName() { return "Rotation3d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<wpi::math::Quaternion>();
  }
  static constexpr std::string_view GetSchema() { return "Quaternion q"; }

  static wpi::math::Rotation3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpi::math::Rotation3d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpi::math::Quaternion>(fn);
  }
};

static_assert(wpi::StructSerializable<wpi::math::Rotation3d>);
static_assert(wpi::HasNestedStruct<wpi::math::Rotation3d>);
