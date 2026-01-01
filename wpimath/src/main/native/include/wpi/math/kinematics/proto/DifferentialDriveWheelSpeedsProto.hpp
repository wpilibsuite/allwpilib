// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/DifferentialDriveWheelSpeeds.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/protobuf_kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT
    wpi::util::Protobuf<wpi::math::DifferentialDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelSpeeds;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::DifferentialDriveWheelSpeeds>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::DifferentialDriveWheelSpeeds>;
  static std::optional<wpi::math::DifferentialDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveWheelSpeeds& value);
};
