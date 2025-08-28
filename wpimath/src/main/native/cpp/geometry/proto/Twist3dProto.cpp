// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/Twist3dProto.h"

#include "wpimath/protobuf/geometry3d.npb.h"

std::optional<wpimath::Twist3d> wpi::Protobuf<wpimath::Twist3d>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufTwist3d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpimath::Twist3d{
      units::meter_t{msg.dx},  units::meter_t{msg.dy},  units::meter_t{msg.dz},
      units::radian_t{msg.rx}, units::radian_t{msg.ry}, units::radian_t{msg.rz},
  };
}

bool wpi::Protobuf<wpimath::Twist3d>::Pack(OutputStream& stream,
                                           const wpimath::Twist3d& value) {
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
