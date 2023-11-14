// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/geometry/Translation2d.h"

template <>
struct wpi::Struct<frc::Translation2d> {
  static constexpr std::string_view kTypeString = "struct:Translation2d";
  static constexpr size_t kSize = 16;
  static constexpr std::string_view kSchema = "double x_meters;double y_meters";

  static frc::Translation2d Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::Translation2d& value);
};
