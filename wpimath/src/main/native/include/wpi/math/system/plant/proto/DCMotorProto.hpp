// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/system/plant/DCMotor.h"
#include "wpimath/protobuf/plant.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::DCMotor> {
  using MessageStruct = wpi_proto_ProtobufDCMotor;
  using InputStream = wpi::ProtoInputStream<frc::DCMotor>;
  using OutputStream = wpi::ProtoOutputStream<frc::DCMotor>;
  static std::optional<frc::DCMotor> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::DCMotor& value);
};
