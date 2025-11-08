// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/proto/Rotation2dProto.hpp"

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpi::math::Rotation2d> wpi::util::Protobuf<wpi::math::Rotation2d>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufRotation2d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::Rotation2d{
      wpi::units::radian_t{msg.value},
  };
}

bool wpi::util::Protobuf<wpi::math::Rotation2d>::Pack(OutputStream& stream,
                                          const wpi::math::Rotation2d& value) {
  wpi_proto_ProtobufRotation2d msg{
      .value = value.Radians().value(),
  };
  return stream.Encode(msg);
}
