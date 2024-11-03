// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/MecanumDriveKinematics.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::MecanumDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveKinematics;
  using InputStream = wpi::ProtoInputStream<frc::MecanumDriveKinematics>;
  using OutputStream = wpi::ProtoOutputStream<frc::MecanumDriveKinematics>;
  static std::optional<frc::MecanumDriveKinematics> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::MecanumDriveKinematics& value);
};
