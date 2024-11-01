// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Twist2dProto.h"

#include "geometry2d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Twist2d>::Message() {
  return get_wpi_proto_ProtobufTwist2d_msg();
}

std::optional<frc::Twist2d> wpi::Protobuf<frc::Twist2d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufTwist2d msg;
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  return frc::Twist2d{
      units::meter_t{msg.dx},
      units::meter_t{msg.dy},
      units::radian_t{msg.dtheta},
  };
}

bool wpi::Protobuf<frc::Twist2d>::Pack(wpi::ProtoOutputStream& stream,
                                       const frc::Twist2d& value) {
  wpi_proto_ProtobufTwist2d msg{
      .dx = value.dx.value(),
      .dy = value.dy.value(),
      .dtheta = value.dtheta.value(),
  };
  return stream.Encode(msg);
}
