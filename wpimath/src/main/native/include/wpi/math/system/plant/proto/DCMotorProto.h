// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/plant.npb.h"
#include "wpi/math/system/plant/DCMotor.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::DCMotor> {
  using MessageStruct = wpi_proto_ProtobufDCMotor;
  using InputStream = wpi::ProtoInputStream<wpi::math::DCMotor>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::DCMotor>;
  static std::optional<wpi::math::DCMotor> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::DCMotor& value);
};
