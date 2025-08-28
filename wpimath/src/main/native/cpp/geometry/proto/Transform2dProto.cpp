// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/proto/Transform2dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpi::math::Transform2d> wpi::Protobuf<wpi::math::Transform2d>::Unpack(
    InputStream& stream) {
  wpi::UnpackCallback<wpi::math::Translation2d> tsln;
  wpi::UnpackCallback<wpi::math::Rotation2d> rot;
  wpi_proto_ProtobufTransform2d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto itsln = tsln.Items();
  auto irot = rot.Items();

  if (itsln.empty() || irot.empty()) {
    return {};
  }

  return wpi::math::Transform2d{
      itsln[0],
      irot[0],
  };
}

bool wpi::Protobuf<wpi::math::Transform2d>::Pack(
    OutputStream& stream, const wpi::math::Transform2d& value) {
  wpi::PackCallback tsln{&value.Translation()};
  wpi::PackCallback rot{&value.Rotation()};
  wpi_proto_ProtobufTransform2d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  return stream.Encode(msg);
}
