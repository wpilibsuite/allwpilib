// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::DifferentialSample> {
  static constexpr std::string_view GetTypeName() {
    return "DifferentialSample";
  }
  static constexpr size_t GetSize() {
    return wpi::util::GetStructSize<wpi::math::HolonomicSample>() + 8 + 8;
  }
  static constexpr std::string_view GetSchema() {
    return "double time;Pose2d pose;ChassisVelocities velocity;"
           "ChassisAccelerations acceleration;double leftVelocity;"
           "double rightVelocity";
  }

  static wpi::math::DifferentialSample Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::DifferentialSample& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::HolonomicSample>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::DifferentialSample>);
static_assert(wpi::util::HasNestedStruct<wpi::math::DifferentialSample>);
