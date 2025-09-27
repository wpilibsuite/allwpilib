// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/trajectory/Trajectory.h"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Trajectory::State> {
  using MessageStruct = wpi_proto_ProtobufTrajectoryState;
  using InputStream = wpi::ProtoInputStream<frc::Trajectory::State>;
  using OutputStream = wpi::ProtoOutputStream<frc::Trajectory::State>;
  static std::optional<frc::Trajectory::State> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Trajectory::State& value);
};
