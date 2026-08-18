// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/HolonomicSample.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::HolonomicSample> {
  static constexpr std::string_view GetTypeName() { return "HolonomicSample"; }
  static constexpr size_t GetSize() {
    return wpi::util::GetStructSize<wpi::math::TrajectorySample>() +
           wpi::util::GetStructSize<wpi::math::Pose2d>() +
           wpi::util::GetStructSize<wpi::math::ChassisVelocities>() +
           wpi::util::GetStructSize<wpi::math::ChassisAccelerations>();
  }
  static constexpr std::string_view GetSchema() {
    return "double time;Pose2d pose;ChassisVelocities velocity;"
           "ChassisAccelerations acceleration";
  }

  static wpi::math::HolonomicSample Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::HolonomicSample& value);
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Pose2d>(fn);
    wpi::util::ForEachStructSchema<wpi::math::ChassisVelocities>(fn);
    wpi::util::ForEachStructSchema<wpi::math::ChassisAccelerations>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::HolonomicSample>);
static_assert(wpi::util::HasNestedStruct<wpi::math::HolonomicSample>);
