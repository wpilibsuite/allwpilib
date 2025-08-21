// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/DifferentialDriveWheelAccelerations.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::DifferentialDriveWheelAccelerations> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelAccelerations;
  using InputStream = wpi::ProtoInputStream<frc::DifferentialDriveWheelAccelerations>;
  using OutputStream = wpi::ProtoOutputStream<frc::DifferentialDriveWheelAccelerations>;
  static std::optional<frc::DifferentialDriveWheelAccelerations> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::DifferentialDriveWheelAccelerations& value);
};
