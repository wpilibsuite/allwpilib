// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/Rectangle2dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpimath::Rectangle2d> wpi::Protobuf<wpimath::Rectangle2d>::Unpack(
    InputStream& stream) {
  wpi::UnpackCallback<wpimath::Pose2d> pose;
  wpi_proto_ProtobufRectangle2d msg{
      .center = pose.Callback(),
      .xWidth = 0,
      .yWidth = 0,
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return wpimath::Rectangle2d{
      ipose[0],
      units::meter_t{msg.xWidth},
      units::meter_t{msg.yWidth},
  };
}

bool wpi::Protobuf<wpimath::Rectangle2d>::Pack(
    OutputStream& stream, const wpimath::Rectangle2d& value) {
  wpi::PackCallback pose{&value.Center()};
  wpi_proto_ProtobufRectangle2d msg{
      .center = pose.Callback(),
      .xWidth = value.XWidth().value(),
      .yWidth = value.YWidth().value(),
  };
  return stream.Encode(msg);
}
