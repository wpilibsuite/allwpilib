// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/QuaternionProto.h"

#include "wpimath/protobuf/geometry3d.npb.h"

std::optional<wpimath::Quaternion> wpi::Protobuf<wpimath::Quaternion>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufQuaternion msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpimath::Quaternion{
      msg.w,
      msg.x,
      msg.y,
      msg.z,
  };
}

bool wpi::Protobuf<wpimath::Quaternion>::Pack(
    OutputStream& stream, const wpimath::Quaternion& value) {
  wpi_proto_ProtobufQuaternion msg{
      .w = value.W(),
      .x = value.X(),
      .y = value.Y(),
      .z = value.Z(),
  };
  return stream.Encode(msg);
}
