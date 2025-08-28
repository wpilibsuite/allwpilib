// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/MecanumDriveWheelSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::MecanumDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelSpeeds;
  using InputStream = wpi::ProtoInputStream<wpimath::MecanumDriveWheelSpeeds>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::MecanumDriveWheelSpeeds>;
  static std::optional<wpimath::MecanumDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::MecanumDriveWheelSpeeds& value);
};
