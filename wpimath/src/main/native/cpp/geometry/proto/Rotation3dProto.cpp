// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Rotation3dProto.h"

#include "geometry3d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Rotation3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufRotation3d>(
      arena);
}

frc::Rotation3d wpi::Protobuf<frc::Rotation3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufRotation3d*>(&msg);
  return frc::Rotation3d{
      wpi::UnpackProtobuf<frc::Quaternion>(m->q()),
  };
}

void wpi::Protobuf<frc::Rotation3d>::Pack(google::protobuf::Message* msg,
                                          const frc::Rotation3d& value) {
  auto m = static_cast<wpi::proto::ProtobufRotation3d*>(msg);
  wpi::PackProtobuf(m->mutable_q(), value.GetQuaternion());
}
