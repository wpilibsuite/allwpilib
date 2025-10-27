// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/geometry/Rectangle2d.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::Rectangle2d> {
  static constexpr std::string_view GetTypeName() { return "Rectangle2d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<frc::Pose2d>() + 16;
  }
  static constexpr std::string_view GetSchema() {
    return "Pose2d center;double xWidth;double yWidth";
  }

  static frc::Rectangle2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::Rectangle2d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Pose2d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::Rectangle2d>);
static_assert(wpi::HasNestedStruct<frc::Rectangle2d>);
