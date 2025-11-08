// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/proto/Rectangle2dProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/geometry2d.npb.h"

std::optional<wpi::math::Rectangle2d> wpi::util::Protobuf<wpi::math::Rectangle2d>::Unpack(
    InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose2d> pose;
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

  return wpi::math::Rectangle2d{
      ipose[0],
      wpi::units::meter_t{msg.xWidth},
      wpi::units::meter_t{msg.yWidth},
  };
}

bool wpi::util::Protobuf<wpi::math::Rectangle2d>::Pack(OutputStream& stream,
                                           const wpi::math::Rectangle2d& value) {
  wpi::util::PackCallback pose{&value.Center()};
  wpi_proto_ProtobufRectangle2d msg{
      .center = pose.Callback(),
      .xWidth = value.XWidth().value(),
      .yWidth = value.YWidth().value(),
  };
  return stream.Encode(msg);
}
