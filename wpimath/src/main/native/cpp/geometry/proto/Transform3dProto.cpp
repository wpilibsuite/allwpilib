// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Transform3dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "geometry3d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Transform3d>::Message() {
  return get_wpi_proto_ProtobufTransform3d_msg();
}

std::optional<frc::Transform3d> wpi::Protobuf<frc::Transform3d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi::UnpackCallback<frc::Translation3d> tsln;
  wpi::UnpackCallback<frc::Rotation3d> rot;
  wpi_proto_ProtobufTransform3d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  auto itsln = tsln.Items();
  auto irot = rot.Items();

  if (itsln.empty() || irot.empty()) {
    return {};
  }

  return frc::Transform3d{
      itsln[0],
      irot[0],
  };
}

bool wpi::Protobuf<frc::Transform3d>::Pack(wpi::ProtoOutputStream& stream,
                                           const frc::Transform3d& value) {
  wpi::PackCallback tsln{&value.Translation()};
  wpi::PackCallback rot{&value.Rotation()};
  wpi_proto_ProtobufTransform3d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  return stream.Encode(msg);
}
