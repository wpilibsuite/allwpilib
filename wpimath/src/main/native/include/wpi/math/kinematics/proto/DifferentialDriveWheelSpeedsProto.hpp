// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/kinematics/DifferentialDriveWheelSpeeds.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/kinematics.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::DifferentialDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelSpeeds;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::DifferentialDriveWheelSpeeds>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::DifferentialDriveWheelSpeeds>;
  static std::optional<wpi::math::DifferentialDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveWheelSpeeds& value);
};
