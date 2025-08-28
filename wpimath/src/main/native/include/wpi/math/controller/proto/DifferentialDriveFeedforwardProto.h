// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/controller/DifferentialDriveFeedforward.h>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::DifferentialDriveFeedforward> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveFeedforward;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::DifferentialDriveFeedforward>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::DifferentialDriveFeedforward>;
  static std::optional<wpi::math::DifferentialDriveFeedforward> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveFeedforward& value);
};
