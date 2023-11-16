// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/SwerveModulePositionProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::SwerveModulePosition>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufSwerveModulePosition>(arena);
}

frc::SwerveModulePosition wpi::Protobuf<frc::SwerveModulePosition>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufSwerveModulePosition*>(&msg);
  return frc::SwerveModulePosition{
      units::meter_t{m->distance()},
      wpi::UnpackProtobuf<frc::Rotation2d>(m->angle()),
  };
}

void wpi::Protobuf<frc::SwerveModulePosition>::Pack(
    google::protobuf::Message* msg, const frc::SwerveModulePosition& value) {
  auto m = static_cast<wpi::proto::ProtobufSwerveModulePosition*>(msg);
  m->set_distance(value.distance.value());
  wpi::PackProtobuf(m->mutable_angle(), value.angle);
}
