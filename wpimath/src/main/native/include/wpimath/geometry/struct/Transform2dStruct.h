// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "wpimath/geometry/Transform2d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpimath::Transform2d> {
  static constexpr std::string_view GetTypeName() { return "Transform2d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<wpimath::Translation2d>() +
           wpi::GetStructSize<wpimath::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation2d translation;Rotation2d rotation";
  }

  static wpimath::Transform2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpimath::Transform2d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpimath::Translation2d>(fn);
    wpi::ForEachStructSchema<wpimath::Rotation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<wpimath::Transform2d>);
static_assert(wpi::HasNestedStruct<wpimath::Transform2d>);
