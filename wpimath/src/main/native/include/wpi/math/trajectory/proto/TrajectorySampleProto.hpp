// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::TrajectorySample> {
  using MessageStruct = wpi_proto_ProtobufTrajectorySample;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::TrajectorySample>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::TrajectorySample>;
  static std::optional<wpi::math::TrajectorySample> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::TrajectorySample& value);
};
