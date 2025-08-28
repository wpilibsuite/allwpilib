// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/proto/Translation2dProto.hpp>

#include "wpimath/protobuf/geometry2d.npb.hpp"

std::optional<wpi::math::Translation2d>
wpi::Protobuf<wpi::math::Translation2d>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufTranslation2d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::Translation2d{
      units::meter_t{msg.x},
      units::meter_t{msg.y},
  };
}

bool wpi::Protobuf<wpi::math::Translation2d>::Pack(
    OutputStream& stream, const wpi::math::Translation2d& value) {
  wpi_proto_ProtobufTranslation2d msg{
      .x = value.X().value(),
      .y = value.Y().value(),
  };
  return stream.Encode(msg);
}
