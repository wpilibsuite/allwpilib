// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Translation3dProto.h"

#include "geometry3d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Translation3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTranslation3d>(arena);
}

frc::Translation3d wpi::Protobuf<frc::Translation3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTranslation3d*>(&msg);
  return frc::Translation3d{
      units::meter_t{m->x()},
      units::meter_t{m->y()},
      units::meter_t{m->z()},
  };
}

void wpi::Protobuf<frc::Translation3d>::Pack(google::protobuf::Message* msg,
                                             const frc::Translation3d& value) {
  auto m = static_cast<wpi::proto::ProtobufTranslation3d*>(msg);
  m->set_x(value.X().value());
  m->set_y(value.Y().value());
  m->set_z(value.Z().value());
}
