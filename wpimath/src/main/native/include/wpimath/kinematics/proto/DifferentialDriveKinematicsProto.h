// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/DifferentialDriveKinematics.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::DifferentialDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveKinematics;
  using InputStream =
      wpi::ProtoInputStream<wpimath::DifferentialDriveKinematics>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::DifferentialDriveKinematics>;
  static std::optional<wpimath::DifferentialDriveKinematics> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::DifferentialDriveKinematics& value);
};
