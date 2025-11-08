// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/proto/Twist3dProto.hpp"

#include "wpimath/protobuf/geometry3d.npb.h"

std::optional<wpi::math::Twist3d> wpi::util::Protobuf<wpi::math::Twist3d>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufTwist3d msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::Twist3d{
      wpi::units::meter_t{msg.dx},  wpi::units::meter_t{msg.dy},  wpi::units::meter_t{msg.dz},
      wpi::units::radian_t{msg.rx}, wpi::units::radian_t{msg.ry}, wpi::units::radian_t{msg.rz},
  };
}

bool wpi::util::Protobuf<wpi::math::Twist3d>::Pack(OutputStream& stream,
                                       const wpi::math::Twist3d& value) {
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
