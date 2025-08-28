// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/kinematics/DifferentialDriveKinematics.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/kinematics.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::DifferentialDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveKinematics;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::DifferentialDriveKinematics>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::DifferentialDriveKinematics>;
  static std::optional<wpi::math::DifferentialDriveKinematics> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveKinematics& value);
};
