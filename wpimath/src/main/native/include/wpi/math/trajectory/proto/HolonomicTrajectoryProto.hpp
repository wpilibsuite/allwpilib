// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/trajectory/HolonomicTrajectory.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::HolonomicTrajectory> {
  using MessageStruct = wpi_proto_ProtobufHolonomicTrajectory;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::HolonomicTrajectory>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::HolonomicTrajectory>;
  static std::optional<wpi::math::HolonomicTrajectory> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::HolonomicTrajectory& value);
};
