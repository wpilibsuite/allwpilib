// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/QuaternionProto.h"

#include "geometry3d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Quaternion>::Message() {
  return get_wpi_proto_ProtobufQuaternion_msg();
}

std::optional<frc::Quaternion> wpi::Protobuf<frc::Quaternion>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufQuaternion msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::Quaternion{
      msg.w,
      msg.x,
      msg.y,
      msg.z,
  };
}

bool wpi::Protobuf<frc::Quaternion>::Pack(wpi::ProtoOutputStream& stream,
                                          const frc::Quaternion& value) {
  wpi_proto_ProtobufQuaternion msg{
      .w = value.W(),
      .x = value.X(),
      .y = value.Y(),
      .z = value.Z(),
  };
  return stream.Encode(msg);
}
