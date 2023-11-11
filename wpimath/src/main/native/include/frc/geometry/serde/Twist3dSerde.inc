// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Twist3d.h"

template <>
struct wpi::Struct<frc::Twist3d> {
  static constexpr std::string_view kTypeString = "struct:Twist3d";
  static constexpr size_t kSize = 48;
  static constexpr std::string_view kSchema =
      "double dx;double dy;double dz;double rx;double ry;double rz";

  static frc::Twist3d Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data, const frc::Twist3d& value);
};

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Twist3d> {
  static constexpr std::string_view kTypeString = "proto:Twist3d";
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Twist3d Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg, const frc::Twist3d& value);
};
