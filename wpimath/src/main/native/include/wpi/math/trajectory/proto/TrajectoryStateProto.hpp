// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/trajectory/Trajectory.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/trajectory.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::Trajectory::State> {
  using MessageStruct = wpi_proto_ProtobufTrajectoryState;
  using InputStream = wpi::ProtoInputStream<wpi::math::Trajectory::State>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::Trajectory::State>;
  static std::optional<wpi::math::Trajectory::State> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::Trajectory::State& value);
};
