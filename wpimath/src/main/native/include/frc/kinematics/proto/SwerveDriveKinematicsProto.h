// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/SwerveDriveKinematics.h"

template <size_t NumModules>
struct wpi::Protobuf<frc::SwerveDriveKinematics<NumModules>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::SwerveDriveKinematics<NumModules> Unpack(
      const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg,
                   const frc::SwerveDriveKinematics<NumModules>& value);
};

#include "frc/kinematics/proto/SwerveDriveKinematicsProto.inc"
