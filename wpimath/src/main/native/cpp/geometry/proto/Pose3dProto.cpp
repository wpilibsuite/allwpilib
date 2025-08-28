// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/Pose3dProto.h"

#include <wpi/protobuf/Protobuf.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/geometry/Pose3d.h"
#include "wpimath/protobuf/geometry3d.npb.h"

std::optional<wpimath::Pose3d> wpi::Protobuf<wpimath::Pose3d>::Unpack(
    InputStream& stream) {
  wpi::UnpackCallback<wpimath::Translation3d> tsln;
  wpi::UnpackCallback<wpimath::Rotation3d> rot;
  wpi_proto_ProtobufPose3d msg{
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

  return wpimath::Pose3d{
      itsln[0],
      irot[0],
  };
}

bool wpi::Protobuf<wpimath::Pose3d>::Pack(OutputStream& stream,
                                          const wpimath::Pose3d& value) {
  wpi::PackCallback tsln{&value.Translation()};
  wpi::PackCallback rot{&value.Rotation()};
  wpi_proto_ProtobufPose3d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  return stream.Encode(msg);
}
