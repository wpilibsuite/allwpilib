// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::apriltag::AprilTag> {
  static constexpr std::string_view GetTypeName() { return "AprilTag"; }
  static constexpr size_t GetSize() {
    return wpi::util::GetStructSize<wpi::math::Pose3d>() + 8;
  }
  static constexpr std::string_view GetSchema() { return "int id;Pose3d pose"; }

  static wpi::apriltag::AprilTag Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::apriltag::AprilTag& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Pose3d>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::apriltag::AprilTag>);
static_assert(wpi::util::HasNestedStruct<wpi::apriltag::AprilTag>);
