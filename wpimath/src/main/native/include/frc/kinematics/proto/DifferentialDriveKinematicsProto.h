// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::DifferentialDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveKinematics;
  using InputStream = wpi::ProtoInputStream<frc::DifferentialDriveKinematics>;
  using OutputStream = wpi::ProtoOutputStream<frc::DifferentialDriveKinematics>;
  static std::optional<frc::DifferentialDriveKinematics> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::DifferentialDriveKinematics& value);
};
