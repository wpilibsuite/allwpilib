// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/proto/Translation3dProto.hpp>

#include "wpimath/protobuf/geometry3d.npb.hpp"

std::optional<wpi::math::Translation3d>
wpi::Protobuf<wpi::math::Translation3d>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufTranslation3d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::Translation3d{
      units::meter_t{msg.x},
      units::meter_t{msg.y},
      units::meter_t{msg.z},
  };
}

bool wpi::Protobuf<wpi::math::Translation3d>::Pack(
    OutputStream& stream, const wpi::math::Translation3d& value) {
  wpi_proto_ProtobufTranslation3d msg{
      .x = value.X().value(),
      .y = value.Y().value(),
      .z = value.Z().value(),
  };
  return stream.Encode(msg);
}
