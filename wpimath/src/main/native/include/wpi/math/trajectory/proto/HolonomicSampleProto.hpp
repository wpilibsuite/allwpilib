// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/HolonomicSample.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::HolonomicSample> {
  using MessageStruct = wpi_proto_ProtobufHolonomicSample;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::HolonomicSample>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::HolonomicSample>;
  static std::optional<wpi::math::HolonomicSample> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::HolonomicSample& value);
};
