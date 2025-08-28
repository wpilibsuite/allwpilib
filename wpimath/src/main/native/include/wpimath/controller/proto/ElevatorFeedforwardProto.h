// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/controller/ElevatorFeedforward.h"
#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::ElevatorFeedforward> {
  using MessageStruct = wpi_proto_ProtobufElevatorFeedforward;
  using InputStream = wpi::ProtoInputStream<wpimath::ElevatorFeedforward>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::ElevatorFeedforward>;
  static std::optional<wpimath::ElevatorFeedforward> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::ElevatorFeedforward& value);
};
