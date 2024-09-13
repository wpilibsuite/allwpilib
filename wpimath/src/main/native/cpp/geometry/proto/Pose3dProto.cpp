// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Pose3dProto.h"

#include "geometry3d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Pose3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufPose3d>(
      arena);
}

frc::Pose3d wpi::Protobuf<frc::Pose3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufPose3d*>(&msg);
  return frc::Pose3d{
      wpi::UnpackProtobuf<frc::Translation3d>(m->translation()),
      wpi::UnpackProtobuf<frc::Rotation3d>(m->rotation()),
  };
}

void wpi::Protobuf<frc::Pose3d>::Pack(google::protobuf::Message* msg,
                                      const frc::Pose3d& value) {
  auto m = static_cast<wpi::proto::ProtobufPose3d*>(msg);
  wpi::PackProtobuf(m->mutable_translation(), value.Translation());
  wpi::PackProtobuf(m->mutable_rotation(), value.Rotation());
}
