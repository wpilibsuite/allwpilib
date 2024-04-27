// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/MecanumDriveKinematicsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::MecanumDriveKinematics>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufMecanumDriveKinematics>(arena);
}

frc::MecanumDriveKinematics wpi::Protobuf<frc::MecanumDriveKinematics>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufMecanumDriveKinematics*>(&msg);
  return frc::MecanumDriveKinematics{
      wpi::UnpackProtobuf<frc::Translation2d>(m->front_left()),
      wpi::UnpackProtobuf<frc::Translation2d>(m->front_right()),
      wpi::UnpackProtobuf<frc::Translation2d>(m->rear_left()),
      wpi::UnpackProtobuf<frc::Translation2d>(m->rear_right()),
  };
}

void wpi::Protobuf<frc::MecanumDriveKinematics>::Pack(
    google::protobuf::Message* msg, const frc::MecanumDriveKinematics& value) {
  auto m = static_cast<wpi::proto::ProtobufMecanumDriveKinematics*>(msg);
  wpi::PackProtobuf(m->mutable_front_left(), value.GetFrontLeft());
  wpi::PackProtobuf(m->mutable_front_right(), value.GetFrontRight());
  wpi::PackProtobuf(m->mutable_rear_left(), value.GetRearLeft());
  wpi::PackProtobuf(m->mutable_rear_right(), value.GetRearRight());
}
