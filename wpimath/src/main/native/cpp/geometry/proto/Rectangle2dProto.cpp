// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Rectangle2dProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "geometry2d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Rectangle2d>::Message() {
  return get_wpi_proto_ProtobufRectangle2d_msg();
}

std::optional<frc::Rectangle2d> wpi::Protobuf<frc::Rectangle2d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi::UnpackCallback<frc::Pose2d> pose;
  wpi_proto_ProtobufRectangle2d msg{
      .center = pose.Callback(),
      .xWidth = 0,
      .yWidth = 0,
  };
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return frc::Rectangle2d{
      ipose[0],
      units::meter_t{msg.xWidth},
      units::meter_t{msg.yWidth},
  };
}

bool wpi::Protobuf<frc::Rectangle2d>::Pack(wpi::ProtoOutputStream& stream,
                                           const frc::Rectangle2d& value) {
  wpi::PackCallback pose{&value.Center()};
  wpi_proto_ProtobufRectangle2d msg{
      .center = pose.Callback(),
      .xWidth = value.XWidth().value(),
      .yWidth = value.YWidth().value(),
  };
  return stream.Encode(msg);
}
