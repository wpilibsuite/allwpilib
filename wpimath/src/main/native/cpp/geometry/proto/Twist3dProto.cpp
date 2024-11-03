// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Twist3dProto.h"

#include "geometry3d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Twist3d>::Message() {
  return get_wpi_proto_ProtobufTwist3d_msg();
}

std::optional<frc::Twist3d> wpi::Protobuf<frc::Twist3d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufTwist3d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::Twist3d{
      units::meter_t{msg.dx},  units::meter_t{msg.dy},  units::meter_t{msg.dz},
      units::radian_t{msg.rx}, units::radian_t{msg.ry}, units::radian_t{msg.rz},
  };
}

bool wpi::Protobuf<frc::Twist3d>::Pack(wpi::ProtoOutputStream& stream,
                                       const frc::Twist3d& value) {
  wpi_proto_ProtobufTwist3d msg{
      .dx = value.dx.value(),
      .dy = value.dy.value(),
      .dz = value.dz.value(),
      .rx = value.rx.value(),
      .ry = value.ry.value(),
      .rz = value.rz.value(),
  };
  return stream.Encode(msg);
}
