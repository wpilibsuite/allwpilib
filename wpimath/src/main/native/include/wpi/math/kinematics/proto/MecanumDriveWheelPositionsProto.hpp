// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/MecanumDriveWheelPositions.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::MecanumDriveWheelPositions> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelPositions;
  using InputStream = wpi::ProtoInputStream<frc::MecanumDriveWheelPositions>;
  using OutputStream = wpi::ProtoOutputStream<frc::MecanumDriveWheelPositions>;
  static std::optional<frc::MecanumDriveWheelPositions> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::MecanumDriveWheelPositions& value);
};
