// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/shape/proto/Ellipse2dProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/shape.npb.h"

std::optional<wpi::math::Ellipse2d>
wpi::util::Protobuf<wpi::math::Ellipse2d>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose2d> pose;
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

  return wpi::math::Ellipse2d{
      ipose[0],
      wpi::units::meters<>{msg.xSemiAxis},
      wpi::units::meters<>{msg.ySemiAxis},
  };
}

bool wpi::util::Protobuf<wpi::math::Ellipse2d>::Pack(
    OutputStream& stream, const wpi::math::Ellipse2d& value) {
  wpi::util::PackCallback pose{&value.Center()};
  wpi_proto_ProtobufEllipse2d msg{
      .center = pose.Callback(),
      .xSemiAxis = value.XSemiAxis().value(),
      .ySemiAxis = value.YSemiAxis().value(),
  };
  return stream.Encode(msg);
}
