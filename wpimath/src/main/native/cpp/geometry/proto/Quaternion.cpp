// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/QuaternionProto.h"

#include "geometry3d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Quaternion>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufQuaternion>(
      arena);
}

frc::Quaternion wpi::Protobuf<frc::Quaternion>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufQuaternion*>(&msg);
  return frc::Quaternion{
      m->w(),
      m->x(),
      m->y(),
      m->z(),
  };
}

void wpi::Protobuf<frc::Quaternion>::Pack(google::protobuf::Message* msg,
                                          const frc::Quaternion& value) {
  auto m = static_cast<wpi::proto::ProtobufQuaternion*>(msg);
  m->set_w(value.W());
  m->set_x(value.X());
  m->set_y(value.Y());
  m->set_z(value.Z());
}
