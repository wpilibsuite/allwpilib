// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/plant.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::DCMotor> {
  using MessageStruct = wpi_proto_ProtobufDCMotor;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::DCMotor>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::DCMotor>;
  static std::optional<wpi::math::DCMotor> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::DCMotor& value);
};
