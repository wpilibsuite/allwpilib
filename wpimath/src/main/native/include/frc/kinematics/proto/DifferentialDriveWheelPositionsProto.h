// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/DifferentialDriveWheelPositions.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::DifferentialDriveWheelPositions> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelPositions;
  using InputStream =
      wpi::ProtoInputStream<frc::DifferentialDriveWheelPositions>;
  using OutputStream =
      wpi::ProtoOutputStream<frc::DifferentialDriveWheelPositions>;
  static std::optional<frc::DifferentialDriveWheelPositions> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::DifferentialDriveWheelPositions& value);
};
