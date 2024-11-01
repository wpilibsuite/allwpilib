// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Ellipse2dProto.h"
#include "wpi/protobuf/ProtobufCallbacks.h"

#include "geometry2d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Ellipse2d>::Message() {
  return get_wpi_proto_ProtobufEllipse2d_msg();
}

std::optional<frc::Ellipse2d> wpi::Protobuf<frc::Ellipse2d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi::UnpackCallback<frc::Pose2d> pose;
  wpi_proto_ProtobufEllipse2d msg{
      .center = pose.Callback(),
  };
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return frc::Ellipse2d{
      ipose[0],
      units::meter_t{msg.xSemiAxis},
      units::meter_t{msg.ySemiAxis},
  };
}

bool wpi::Protobuf<frc::Ellipse2d>::Pack(wpi::ProtoOutputStream& stream,
                                         const frc::Ellipse2d& value) {
  wpi::PackCallback pose{&value.Center()};
  wpi_proto_ProtobufEllipse2d msg{
      .center = pose.Callback(),
      .xSemiAxis = value.XSemiAxis().value(),
      .ySemiAxis = value.YSemiAxis().value(),
  };
  return stream.Encode(msg);
}
