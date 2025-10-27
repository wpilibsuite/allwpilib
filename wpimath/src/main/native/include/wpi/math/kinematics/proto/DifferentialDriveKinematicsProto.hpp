// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/protobuf/Protobuf.hpp>

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::DifferentialDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveKinematics;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::DifferentialDriveKinematics>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::DifferentialDriveKinematics>;
  static std::optional<wpi::math::DifferentialDriveKinematics> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveKinematics& value);
};
