// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Rotation2d.h"

template <>
struct wpi::Struct<frc::Rotation2d> {
  static constexpr std::string_view kTypeString = "struct:Rotation2d";
  static constexpr size_t kSize = 8;
  static constexpr std::string_view kSchema = "double value";

  static frc::Rotation2d Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::Rotation2d& value);
};

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Rotation2d> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Rotation2d Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg,
                   const frc::Rotation2d& value);
};
