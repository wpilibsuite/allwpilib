// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Ellipse2d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Ellipse2d> {
  static constexpr std::string_view GetTypeName() { return "Ellipse2d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<frc::Pose2d>() + 16;
  }
  static constexpr std::string_view GetSchema() {
    return "Pose2d center;double xSemiAxis;double ySemiAxis";
  }

  static frc::Ellipse2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::Ellipse2d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Pose2d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::Ellipse2d>);
static_assert(wpi::HasNestedStruct<frc::Ellipse2d>);
