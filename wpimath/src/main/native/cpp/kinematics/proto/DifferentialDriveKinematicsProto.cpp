// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveKinematicsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::DifferentialDriveKinematics>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufDifferentialDriveKinematics>(arena);
}

frc::DifferentialDriveKinematics
wpi::Protobuf<frc::DifferentialDriveKinematics>::Unpack(
    const google::protobuf::Message& msg) {
  auto m =
      static_cast<const wpi::proto::ProtobufDifferentialDriveKinematics*>(&msg);
  return frc::DifferentialDriveKinematics{
      units::meter_t{m->track_width()},
  };
}

void wpi::Protobuf<frc::DifferentialDriveKinematics>::Pack(
    google::protobuf::Message* msg,
    const frc::DifferentialDriveKinematics& value) {
  auto m = static_cast<wpi::proto::ProtobufDifferentialDriveKinematics*>(msg);
  m->set_track_width(value.trackWidth.value());
}
