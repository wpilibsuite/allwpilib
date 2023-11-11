// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Quaternion.h"

template <>
struct wpi::Struct<frc::Quaternion> {
  static constexpr std::string_view kTypeString = "struct:Quaternion";
  static constexpr size_t kSize = 32;
  static constexpr std::string_view kSchema =
      "double w;double x;double y;double z";

  static frc::Quaternion Unpack(std::span<const uint8_t, kSize> data);
  static void Pack(std::span<uint8_t, kSize> data,
                   const frc::Quaternion& value);
};

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Quaternion> {
  static constexpr std::string_view kTypeString = "proto:Quaternion";
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Quaternion Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg,
                   const frc::Quaternion& value);
};
