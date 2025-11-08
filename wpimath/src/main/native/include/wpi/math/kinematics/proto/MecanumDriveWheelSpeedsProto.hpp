// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/MecanumDriveWheelSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::MecanumDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelSpeeds;
  using InputStream = wpi::ProtoInputStream<frc::MecanumDriveWheelSpeeds>;
  using OutputStream = wpi::ProtoOutputStream<frc::MecanumDriveWheelSpeeds>;
  static std::optional<frc::MecanumDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::MecanumDriveWheelSpeeds& value);
};
