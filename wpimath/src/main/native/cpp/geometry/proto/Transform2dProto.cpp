// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Transform2dProto.h"

#include "geometry2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Transform2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTransform2d>(arena);
}

frc::Transform2d wpi::Protobuf<frc::Transform2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTransform2d*>(&msg);
  return frc::Transform2d{
      wpi::UnpackProtobuf<frc::Translation2d>(m->translation()),
      wpi::UnpackProtobuf<frc::Rotation2d>(m->rotation()),
  };
}

void wpi::Protobuf<frc::Transform2d>::Pack(google::protobuf::Message* msg,
                                           const frc::Transform2d& value) {
  auto m = static_cast<wpi::proto::ProtobufTransform2d*>(msg);
  wpi::PackProtobuf(m->mutable_translation(), value.Translation());
  wpi::PackProtobuf(m->mutable_rotation(), value.Rotation());
}
