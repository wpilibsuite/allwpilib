// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/MecanumDriveWheelAccelerations.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::MecanumDriveWheelAccelerations> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelAccelerations;
  using InputStream = wpi::ProtoInputStream<frc::MecanumDriveWheelAccelerations>;
  using OutputStream = wpi::ProtoOutputStream<frc::MecanumDriveWheelAccelerations>;
  static std::optional<frc::MecanumDriveWheelAccelerations> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::MecanumDriveWheelAccelerations& value);
};
