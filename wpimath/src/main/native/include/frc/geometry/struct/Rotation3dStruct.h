// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Rotation3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Rotation3d> {
  static constexpr std::string_view GetTypeString() {
    return "struct:Rotation3d";
  }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<frc::Quaternion>();
  }
  static constexpr std::string_view GetSchema() { return "Quaternion q"; }

  static frc::Rotation3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::Rotation3d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Quaternion>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::Rotation3d>);
static_assert(wpi::HasNestedStruct<frc::Rotation3d>);
