// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Trajectory::State> {
  using MessageStruct = wpi_proto_ProtobufTrajectoryState;
  using InputStream = wpi::ProtoInputStream<frc::Trajectory::State>;
  using OutputStream = wpi::ProtoOutputStream<frc::Trajectory::State>;
  static std::optional<frc::Trajectory::State> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Trajectory::State& value);
};
