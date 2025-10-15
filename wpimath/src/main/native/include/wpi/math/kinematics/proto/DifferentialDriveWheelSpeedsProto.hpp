// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::DifferentialDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelSpeeds;
  using InputStream = wpi::ProtoInputStream<frc::DifferentialDriveWheelSpeeds>;
  using OutputStream =
      wpi::ProtoOutputStream<frc::DifferentialDriveWheelSpeeds>;
  static std::optional<frc::DifferentialDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::DifferentialDriveWheelSpeeds& value);
};
