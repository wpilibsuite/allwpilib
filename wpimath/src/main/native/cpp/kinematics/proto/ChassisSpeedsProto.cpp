// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/ChassisSpeedsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::ChassisSpeeds>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufChassisSpeeds>(arena);
}

frc::ChassisSpeeds wpi::Protobuf<frc::ChassisSpeeds>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufChassisSpeeds*>(&msg);
  return frc::ChassisSpeeds{
      units::meters_per_second_t{m->vx()},
      units::meters_per_second_t{m->vy()},
      units::radians_per_second_t{m->omega()},
  };
}

void wpi::Protobuf<frc::ChassisSpeeds>::Pack(google::protobuf::Message* msg,
                                             const frc::ChassisSpeeds& value) {
  auto m = static_cast<wpi::proto::ProtobufChassisSpeeds*>(msg);
  m->set_vx(value.vx.value());
  m->set_vy(value.vy.value());
  m->set_omega(value.omega.value());
}
