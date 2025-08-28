// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/trajectory/Trajectory.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/trajectory.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::Trajectory> {
  using MessageStruct = wpi_proto_ProtobufTrajectory;
  using InputStream = wpi::ProtoInputStream<wpi::math::Trajectory>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::Trajectory>;
  static std::optional<wpi::math::Trajectory> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Trajectory& value);
};
