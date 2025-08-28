// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpimath/controller/DifferentialDriveFeedforward.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::DifferentialDriveFeedforward> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveFeedforward;
  using InputStream =
      wpi::ProtoInputStream<wpimath::DifferentialDriveFeedforward>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::DifferentialDriveFeedforward>;
  static std::optional<wpimath::DifferentialDriveFeedforward> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::DifferentialDriveFeedforward& value);
};
