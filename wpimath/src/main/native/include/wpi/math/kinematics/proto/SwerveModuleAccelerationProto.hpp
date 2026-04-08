// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/SwerveModuleAcceleration.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT
    wpi::util::Protobuf<wpi::math::SwerveModuleAcceleration> {
  using MessageStruct = wpi_proto_ProtobufSwerveModuleAcceleration;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::SwerveModuleAcceleration>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::SwerveModuleAcceleration>;
  static std::optional<wpi::math::SwerveModuleAcceleration> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::SwerveModuleAcceleration& value);
};
