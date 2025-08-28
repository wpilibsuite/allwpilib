// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/plant.npb.h"
#include "wpimath/system/plant/DCMotor.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::DCMotor> {
  using MessageStruct = wpi_proto_ProtobufDCMotor;
  using InputStream = wpi::ProtoInputStream<wpimath::DCMotor>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::DCMotor>;
  static std::optional<wpimath::DCMotor> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::DCMotor& value);
};
