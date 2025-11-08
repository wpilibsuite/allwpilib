// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/trajectory/Trajectory.h"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Trajectory> {
  using MessageStruct = wpi_proto_ProtobufTrajectory;
  using InputStream = wpi::ProtoInputStream<frc::Trajectory>;
  using OutputStream = wpi::ProtoOutputStream<frc::Trajectory>;
  static std::optional<frc::Trajectory> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Trajectory& value);
};
