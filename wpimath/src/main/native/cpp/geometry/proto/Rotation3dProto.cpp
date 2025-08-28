// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/proto/Rotation3dProto.hpp>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/geometry3d.npb.hpp"

std::optional<wpi::math::Rotation3d>
wpi::Protobuf<wpi::math::Rotation3d>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<wpi::math::Quaternion> quat;
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

  return wpi::math::Rotation3d{
      iquat[0],
  };
}

bool wpi::Protobuf<wpi::math::Rotation3d>::Pack(
    OutputStream& stream, const wpi::math::Rotation3d& value) {
  wpi::PackCallback quat{&value.GetQuaternion()};
  wpi_proto_ProtobufRotation3d msg{
      .q = quat.Callback(),
  };
  return stream.Encode(msg);
}
