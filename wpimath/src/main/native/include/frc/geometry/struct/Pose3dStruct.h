// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Pose3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Pose3d> {
  static constexpr std::string_view GetTypeName() { return "Pose3d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<frc::Translation3d>() +
           wpi::GetStructSize<frc::Rotation3d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation3d translation;Rotation3d rotation";
  }

  static frc::Pose3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::Pose3d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Translation3d>(fn);
    wpi::ForEachStructSchema<frc::Rotation3d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::Pose3d>);
static_assert(wpi::HasNestedStruct<frc::Pose3d>);
