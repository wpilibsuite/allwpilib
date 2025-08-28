// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpi/math/kinematics/MecanumDriveKinematics.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::MecanumDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveKinematics;
  using InputStream = wpi::ProtoInputStream<wpi::math::MecanumDriveKinematics>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::MecanumDriveKinematics>;
  static std::optional<wpi::math::MecanumDriveKinematics> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::MecanumDriveKinematics& value);
};
