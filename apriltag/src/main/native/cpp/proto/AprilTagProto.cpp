// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/proto/AprilTagProto.hpp"

#include "apriltag/protobuf/apriltag.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<wpi::apriltag::AprilTag>
wpi::util::Protobuf<wpi::apriltag::AprilTag>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose3d> pose;
  wpi_proto_ProtobufAprilTag msg{
      .id = 0,
      .pose = pose.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return wpi::apriltag::AprilTag{
      static_cast<int>(msg.id),
      ipose[0],
  };
}

bool wpi::util::Protobuf<wpi::apriltag::AprilTag>::Pack(
    OutputStream& stream, const wpi::apriltag::AprilTag& value) {
  wpi::util::PackCallback pose{&value.pose};
  wpi_proto_ProtobufAprilTag msg{
      .id = static_cast<uint32_t>(value.ID),
      .pose = pose.Callback(),
  };
  return stream.Encode(msg);
}
