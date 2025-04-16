// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/ElevatorFeedforward.h"
#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::ElevatorFeedforward<Input>> {
  using MessageStruct = wpi_proto_ProtobufElevatorFeedforward;
  using InputStream = wpi::ProtoInputStream<frc::ElevatorFeedforward<Input>>;
  using OutputStream = wpi::ProtoOutputStream<frc::ElevatorFeedforward<Input>>;
  static std::optional<frc::ElevatorFeedforward<Input>> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::ElevatorFeedforward<Input>& value);
};
