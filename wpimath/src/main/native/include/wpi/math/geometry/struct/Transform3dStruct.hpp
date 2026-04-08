// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::Transform3d> {
  static constexpr std::string_view GetTypeName() { return "Transform3d"; }
  static constexpr size_t GetSize() {
    return wpi::util::GetStructSize<wpi::math::Translation3d>() +
           wpi::util::GetStructSize<wpi::math::Rotation3d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation3d translation;Rotation3d rotation";
  }

  static wpi::math::Transform3d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::Transform3d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Translation3d>(fn);
    wpi::util::ForEachStructSchema<wpi::math::Rotation3d>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::Transform3d>);
static_assert(wpi::util::HasNestedStruct<wpi::math::Transform3d>);
