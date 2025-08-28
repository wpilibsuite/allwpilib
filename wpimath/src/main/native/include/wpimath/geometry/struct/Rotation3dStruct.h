// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "wpimath/geometry/Rotation3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpimath::Rotation3d> {
  static constexpr std::string_view GetTypeName() { return "Rotation3d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<wpimath::Quaternion>();
  }
  static constexpr std::string_view GetSchema() { return "Quaternion q"; }

  static wpimath::Rotation3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpimath::Rotation3d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpimath::Quaternion>(fn);
  }
};

static_assert(wpi::StructSerializable<wpimath::Rotation3d>);
static_assert(wpi::HasNestedStruct<wpimath::Rotation3d>);
