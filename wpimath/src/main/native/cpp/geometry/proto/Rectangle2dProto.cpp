// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Rectangle2dProto.h"

#include <wpi/ProtoHelper.h>

#include "geometry2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Rectangle2d>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufRectangle2d>(arena);
}

frc::Rectangle2d wpi::Protobuf<frc::Rectangle2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufRectangle2d*>(&msg);
  return frc::Rectangle2d{
      wpi::UnpackProtobuf<frc::Pose2d>(m->center()),
      units::meter_t{m->xwidth()},
      units::meter_t{m->ywidth()},
  };
}

void wpi::Protobuf<frc::Rectangle2d>::Pack(google::protobuf::Message* msg,
                                           const frc::Rectangle2d& value) {
  auto m = static_cast<wpi::proto::ProtobufRectangle2d*>(msg);
  wpi::PackProtobuf(m->mutable_center(), value.Center());
  m->set_xwidth(value.XWidth().value());
  m->set_ywidth(value.YWidth().value());
}
