// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/DifferentialDriveWheelSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::DifferentialDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelSpeeds;
  using InputStream =
      wpi::ProtoInputStream<wpimath::DifferentialDriveWheelSpeeds>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::DifferentialDriveWheelSpeeds>;
  static std::optional<wpimath::DifferentialDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::DifferentialDriveWheelSpeeds& value);
};
