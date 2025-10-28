// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/protobuf/Protobuf.hpp>

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::Trajectory::State> {
  using MessageStruct = wpi_proto_ProtobufTrajectoryState;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::Trajectory::State>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::Trajectory::State>;
  static std::optional<wpi::math::Trajectory::State> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::Trajectory::State& value);
};
