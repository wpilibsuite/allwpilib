// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/MecanumDriveWheelPositions.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::MecanumDriveWheelPositions> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelPositions;
  using InputStream =
      wpi::ProtoInputStream<wpimath::MecanumDriveWheelPositions>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::MecanumDriveWheelPositions>;
  static std::optional<wpimath::MecanumDriveWheelPositions> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::MecanumDriveWheelPositions& value);
};
