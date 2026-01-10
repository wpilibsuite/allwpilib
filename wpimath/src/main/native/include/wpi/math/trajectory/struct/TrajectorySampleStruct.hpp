// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::math::TrajectorySample> {
  static constexpr std::string_view GetTypeName() { return "TrajectorySample"; }
  static constexpr size_t GetSize() { return 88; }
  static constexpr std::string_view GetSchema() {
    return "double timestamp;Pose2d pose;ChassisSpeeds velocity;"
           "ChassisAccelerations acceleration";
  }

  static wpi::math::TrajectorySample Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpi::math::TrajectorySample& value);
};

static_assert(wpi::util::StructSerializable<wpi::math::TrajectorySample>);
