// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/Rotation3dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/geometry3d.npb.h"

std::optional<wpimath::Rotation3d> wpi::Protobuf<wpimath::Rotation3d>::Unpack(
    InputStream& stream) {
  wpi::UnpackCallback<wpimath::Quaternion> quat;
  wpi_proto_ProtobufRotation3d msg{
      .q = quat.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iquat = quat.Items();

  if (iquat.empty()) {
    return {};
  }

  return wpimath::Rotation3d{
      iquat[0],
  };
}

bool wpi::Protobuf<wpimath::Rotation3d>::Pack(
    OutputStream& stream, const wpimath::Rotation3d& value) {
  wpi::PackCallback quat{&value.GetQuaternion()};
  wpi_proto_ProtobufRotation3d msg{
      .q = quat.Callback(),
  };
  return stream.Encode(msg);
}
