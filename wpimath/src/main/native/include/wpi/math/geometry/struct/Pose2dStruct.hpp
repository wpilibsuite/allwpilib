// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::Pose2d> {
  static constexpr std::string_view GetTypeName() { return "Pose2d"; }
  static constexpr size_t GetSize() {
    return wpi::util::GetStructSize<wpi::math::Translation2d>() +
           wpi::util::GetStructSize<wpi::math::Rotation2d>();
  }
  static constexpr std::string_view GetSchema() {
    return "Translation2d translation;Rotation2d rotation";
  }

  static wpi::math::Pose2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpi::math::Pose2d& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Translation2d>(fn);
    wpi::util::ForEachStructSchema<wpi::math::Rotation2d>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::Pose2d>);
static_assert(wpi::util::HasNestedStruct<wpi::math::Pose2d>);
