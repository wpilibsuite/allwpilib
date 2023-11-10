// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/controller/SimpleMotorFeedforward.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters>

template <class Distance>
struct wpi::Struct<frc::SimpleMotorFeedforward<Distance>> {
  static constexpr std::string_view kTypeString =
      "struct:SimpleMotorFeedforward";
  static constexpr size_t kSize = 24;
  static constexpr std::string_view kSchema = "double ks;double kv;double ka";

  static frc::SimpleMotorFeedforward<Distance> Unpack(
      std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::SimpleMotorFeedforward<Distance>& value);
};

#include "frc/controller/struct/SimpleMotorFeedforwardStruct.inc"
