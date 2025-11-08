// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Trajectory> {
  using MessageStruct = wpi_proto_ProtobufTrajectory;
  using InputStream = wpi::ProtoInputStream<frc::Trajectory>;
  using OutputStream = wpi::ProtoOutputStream<frc::Trajectory>;
  static std::optional<frc::Trajectory> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Trajectory& value);
};
