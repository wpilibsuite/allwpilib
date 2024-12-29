// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Transform2dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<frc::Transform2d> wpi::Protobuf<frc::Transform2d>::Unpack(
    InputStream& stream) {
  wpi::UnpackCallback<frc::Translation2d> tsln;
  wpi::UnpackCallback<frc::Rotation2d> rot;
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

  return frc::Transform2d{
      itsln[0],
      irot[0],
  };
}

bool wpi::Protobuf<frc::Transform2d>::Pack(OutputStream& stream,
                                           const frc::Transform2d& value) {
  wpi::PackCallback tsln{&value.Translation()};
  wpi::PackCallback rot{&value.Rotation()};
  wpi_proto_ProtobufTransform2d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  return stream.Encode(msg);
}
