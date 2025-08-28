// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/proto/Ellipse2dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpimath::Ellipse2d> wpi::Protobuf<wpimath::Ellipse2d>::Unpack(
    InputStream& stream) {
  wpi::UnpackCallback<wpimath::Pose2d> pose;
  wpi_proto_ProtobufEllipse2d msg{
      .center = pose.Callback(),
      .xSemiAxis = 0,
      .ySemiAxis = 0,
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return wpimath::Ellipse2d{
      ipose[0],
      units::meter_t{msg.xSemiAxis},
      units::meter_t{msg.ySemiAxis},
  };
}

bool wpi::Protobuf<wpimath::Ellipse2d>::Pack(OutputStream& stream,
                                             const wpimath::Ellipse2d& value) {
  wpi::PackCallback pose{&value.Center()};
  wpi_proto_ProtobufEllipse2d msg{
      .center = pose.Callback(),
      .xSemiAxis = value.XSemiAxis().value(),
      .ySemiAxis = value.YSemiAxis().value(),
  };
  return stream.Encode(msg);
}
