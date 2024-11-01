// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Translation3dProto.h"

#include "geometry3d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Translation3d>::Message() {
  return get_wpi_proto_ProtobufTranslation3d_msg();
}

std::optional<frc::Translation3d> wpi::Protobuf<frc::Translation3d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufTranslation3d msg;
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  return frc::Translation3d{
      units::meter_t{msg.x},
      units::meter_t{msg.y},
      units::meter_t{msg.z},
  };
}

bool wpi::Protobuf<frc::Translation3d>::Pack(wpi::ProtoOutputStream& stream,
                                             const frc::Translation3d& value) {
  wpi_proto_ProtobufTranslation3d msg{
      .x = value.X().value(),
      .y = value.Y().value(),
      .z = value.Z().value(),
  };
  return stream.Encode(msg);
}
