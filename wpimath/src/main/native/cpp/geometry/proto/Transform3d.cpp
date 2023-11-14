// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Transform3dProto.h"

#include "geometry3d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Transform3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTransform3d>(arena);
}

frc::Transform3d wpi::Protobuf<frc::Transform3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTransform3d*>(&msg);
  return frc::Transform3d{
      wpi::UnpackProtobuf<frc::Translation3d>(m->translation()),
      wpi::UnpackProtobuf<frc::Rotation3d>(m->rotation()),
  };
}

void wpi::Protobuf<frc::Transform3d>::Pack(google::protobuf::Message* msg,
                                           const frc::Transform3d& value) {
  auto m = static_cast<wpi::proto::ProtobufTransform3d*>(msg);
  wpi::PackProtobuf(m->mutable_translation(), value.Translation());
  wpi::PackProtobuf(m->mutable_rotation(), value.Rotation());
}
