// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/trajectory.npb.h"
#include "wpimath/trajectory/Trajectory.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::Trajectory> {
  using MessageStruct = wpi_proto_ProtobufTrajectory;
  using InputStream = wpi::ProtoInputStream<wpimath::Trajectory>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::Trajectory>;
  static std::optional<wpimath::Trajectory> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::Trajectory& value);
};
