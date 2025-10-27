// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/ChassisSpeeds.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::ChassisSpeeds> {
  static constexpr std::string_view GetTypeName() { return "ChassisSpeeds"; }
  static constexpr size_t GetSize() { return 24; }
  static constexpr std::string_view GetSchema() {
    return "double vx;double vy;double omega";
  }

  static frc::ChassisSpeeds Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::ChassisSpeeds& value);
};

static_assert(wpi::StructSerializable<frc::ChassisSpeeds>);
