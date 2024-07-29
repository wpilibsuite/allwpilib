// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/geometry/Pose3d.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Pose3d> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Pose3d Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg, const frc::Pose3d& value);
};
