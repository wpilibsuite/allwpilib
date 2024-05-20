// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/SwerveModuleStateProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::SwerveModuleState>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufSwerveModuleState>(arena);
}

frc::SwerveModuleState wpi::Protobuf<frc::SwerveModuleState>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufSwerveModuleState*>(&msg);
  return frc::SwerveModuleState{
      units::meters_per_second_t{m->speed()},
      wpi::UnpackProtobuf<frc::Rotation2d>(m->angle()),
  };
}

void wpi::Protobuf<frc::SwerveModuleState>::Pack(
    google::protobuf::Message* msg, const frc::SwerveModuleState& value) {
  auto m = static_cast<wpi::proto::ProtobufSwerveModuleState*>(msg);
  m->set_speed(value.speed.value());
  wpi::PackProtobuf(m->mutable_angle(), value.angle);
}
