// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Rotation2dProto.h"

#include "geometry2d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Rotation2d>::Message() {
  return get_wpi_proto_ProtobufRotation2d_msg();
}

std::optional<frc::Rotation2d> wpi::Protobuf<frc::Rotation2d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufRotation2d msg;
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  return frc::Rotation2d{
      units::radian_t{msg.value},
  };
}

bool wpi::Protobuf<frc::Rotation2d>::Pack(wpi::ProtoOutputStream& stream,
                                          const frc::Rotation2d& value) {
  wpi_proto_ProtobufRotation2d msg{
      .value = value.Radians().value(),
  };
  return stream.Encode(msg);
}
