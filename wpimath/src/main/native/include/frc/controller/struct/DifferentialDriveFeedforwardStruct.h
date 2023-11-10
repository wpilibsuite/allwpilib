// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/controller/DifferentialDriveFeedforward.h"

template <>
struct wpi::Struct<frc::DifferentialDriveFeedforward> {
  static constexpr std::string_view kTypeString =
      "struct:DifferentialDriveFeedforward";
  static constexpr size_t kSize = 32;
  static constexpr std::string_view kSchema =
      "double kv_linear;double ka_linear;double kv_angular;double ka_angular";

  static frc::DifferentialDriveFeedforward Unpack(
      std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::DifferentialDriveFeedforward& value);
};
