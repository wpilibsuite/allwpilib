// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/Twist2dProto.h"

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpimath::Twist2d> wpi::Protobuf<wpimath::Twist2d>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufTwist2d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpimath::Twist2d{
      units::meter_t{msg.dx},
      units::meter_t{msg.dy},
      units::radian_t{msg.dtheta},
  };
}

bool wpi::Protobuf<wpimath::Twist2d>::Pack(OutputStream& stream,
                                           const wpimath::Twist2d& value) {
  wpi_proto_ProtobufTwist2d msg{
      .dx = value.dx.value(),
      .dy = value.dy.value(),
      .dtheta = value.dtheta.value(),
  };
  return stream.Encode(msg);
}
