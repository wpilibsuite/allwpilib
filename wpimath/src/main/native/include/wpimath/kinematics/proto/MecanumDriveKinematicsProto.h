// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/MecanumDriveKinematics.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::MecanumDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveKinematics;
  using InputStream = wpi::ProtoInputStream<wpimath::MecanumDriveKinematics>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::MecanumDriveKinematics>;
  static std::optional<wpimath::MecanumDriveKinematics> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::MecanumDriveKinematics& value);
};
