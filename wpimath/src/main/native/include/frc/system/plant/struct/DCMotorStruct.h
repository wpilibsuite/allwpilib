// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/system/plant/DCMotor.h"

template <>
struct wpi::Struct<frc::DCMotor> {
  static constexpr std::string_view kTypeString = "struct:DCMotor";
  static constexpr size_t kSize = 64;
  static constexpr std::string_view kSchema =
      "double nominal_voltage;double stall_torque;double stall_current;double "
      "free_current;double free_speed;double r;double kv;double kt";

  static frc::DCMotor Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data, const frc::DCMotor& value);
};
