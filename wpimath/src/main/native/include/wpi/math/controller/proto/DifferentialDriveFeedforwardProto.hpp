// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/controller/DifferentialDriveFeedforward.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::DifferentialDriveFeedforward> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveFeedforward;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::DifferentialDriveFeedforward>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::DifferentialDriveFeedforward>;
  static std::optional<wpi::math::DifferentialDriveFeedforward> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::DifferentialDriveFeedforward& value);
};
