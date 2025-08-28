// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/controller/ElevatorFeedforward.h>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::ElevatorFeedforward> {
  using MessageStruct = wpi_proto_ProtobufElevatorFeedforward;
  using InputStream = wpi::ProtoInputStream<wpi::math::ElevatorFeedforward>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::ElevatorFeedforward>;
  static std::optional<wpi::math::ElevatorFeedforward> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::ElevatorFeedforward& value);
};
