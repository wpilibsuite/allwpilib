// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/proto/Twist2dProto.hpp"

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpi::math::Twist2d> wpi::util::Protobuf<wpi::math::Twist2d>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufTwist2d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::Twist2d{
      wpi::units::meter_t{msg.dx},
      wpi::units::meter_t{msg.dy},
      wpi::units::radian_t{msg.dtheta},
  };
}

bool wpi::util::Protobuf<wpi::math::Twist2d>::Pack(OutputStream& stream,
                                       const wpi::math::Twist2d& value) {
  wpi_proto_ProtobufTwist2d msg{
      .dx = value.dx.value(),
      .dy = value.dy.value(),
      .dtheta = value.dtheta.value(),
  };
  return stream.Encode(msg);
}
