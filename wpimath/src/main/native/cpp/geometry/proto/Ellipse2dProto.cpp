// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Ellipse2dProto.h"

#include <wpi/ProtoHelper.h>

#include "geometry2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Ellipse2d>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufEllipse2d>(arena);
}

frc::Ellipse2d wpi::Protobuf<frc::Ellipse2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufEllipse2d*>(&msg);
  return frc::Ellipse2d{
      wpi::UnpackProtobuf<frc::Pose2d>(m->center()),
      units::meter_t{m->xsemiaxis()},
      units::meter_t{m->ysemiaxis()},
  };
}

void wpi::Protobuf<frc::Ellipse2d>::Pack(google::protobuf::Message* msg,
                                         const frc::Ellipse2d& value) {
  auto m = static_cast<wpi::proto::ProtobufEllipse2d*>(msg);
  wpi::PackProtobuf(m->mutable_center(), value.Center());
  m->set_xsemiaxis(value.XSemiAxis().value());
  m->set_ysemiaxis(value.YSemiAxis().value());
}
