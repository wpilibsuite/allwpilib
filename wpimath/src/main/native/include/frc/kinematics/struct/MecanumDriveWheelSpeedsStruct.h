// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/kinematics/MecanumDriveWheelSpeeds.h"

template <>
struct wpi::Struct<frc::MecanumDriveWheelSpeeds> {
  static constexpr std::string_view kTypeString =
      "struct:MecanumDriveWheelSpeeds";
  static constexpr size_t kSize = 32;
  static constexpr std::string_view kSchema =
      "double front_left;double front_right;double rear_left;double rear_right";

  static frc::MecanumDriveWheelSpeeds Unpack(
      std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::MecanumDriveWheelSpeeds& value);
};
