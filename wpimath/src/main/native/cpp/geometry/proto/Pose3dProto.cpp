// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/proto/Pose3dProto.hpp"

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/geometry3d.npb.h"

std::optional<wpi::math::Pose3d> wpi::util::Protobuf<wpi::math::Pose3d>::Unpack(
    InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Translation3d> tsln;
  wpi::util::UnpackCallback<wpi::math::Rotation3d> rot;
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

  return wpi::math::Pose3d{
      itsln[0],
      irot[0],
  };
}

bool wpi::util::Protobuf<wpi::math::Pose3d>::Pack(OutputStream& stream,
                                      const wpi::math::Pose3d& value) {
  wpi::util::PackCallback tsln{&value.Translation()};
  wpi::util::PackCallback rot{&value.Rotation()};
  wpi_proto_ProtobufPose3d msg{
      .translation = tsln.Callback(),
      .rotation = rot.Callback(),
  };
  return stream.Encode(msg);
}
