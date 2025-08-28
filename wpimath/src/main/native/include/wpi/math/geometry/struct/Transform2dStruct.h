// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/geometry/Transform2d.h>
#include <wpi/struct/Struct.h>

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpi::math::Transform2d> {
  static constexpr std::string_view GetTypeName() { return "Transform2d"; }
  static constexpr size_t GetSize() {
    return wpi::GetStructSize<wpi::math::Translation2d>() +
           wpi::GetStructSize<wpi::math::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation2d translation;Rotation2d rotation";
  }

  static wpi::math::Transform2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::Transform2d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpi::math::Translation2d>(fn);
    wpi::ForEachStructSchema<wpi::math::Rotation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<wpi::math::Transform2d>);
static_assert(wpi::HasNestedStruct<wpi::math::Transform2d>);
