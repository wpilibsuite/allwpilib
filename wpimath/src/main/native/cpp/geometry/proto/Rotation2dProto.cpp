// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Rotation2dProto.h"

#include "geometry2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Rotation2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufRotation2d>(
      arena);
}

frc::Rotation2d wpi::Protobuf<frc::Rotation2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufRotation2d*>(&msg);
  return frc::Rotation2d{
      units::radian_t{m->value()},
  };
}

void wpi::Protobuf<frc::Rotation2d>::Pack(google::protobuf::Message* msg,
                                          const frc::Rotation2d& value) {
  auto m = static_cast<wpi::proto::ProtobufRotation2d*>(msg);
  m->set_value(value.Radians().value());
}
