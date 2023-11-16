// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/MecanumDriveWheelSpeedsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::MecanumDriveWheelSpeeds>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufMecanumDriveWheelSpeeds>(arena);
}

frc::MecanumDriveWheelSpeeds
wpi::Protobuf<frc::MecanumDriveWheelSpeeds>::Unpack(
    const google::protobuf::Message& msg) {
  auto m =
      static_cast<const wpi::proto::ProtobufMecanumDriveWheelSpeeds*>(&msg);
  return frc::MecanumDriveWheelSpeeds{
      units::meters_per_second_t{m->front_left()},
      units::meters_per_second_t{m->front_right()},
      units::meters_per_second_t{m->rear_left()},
      units::meters_per_second_t{m->rear_right()},
  };
}

void wpi::Protobuf<frc::MecanumDriveWheelSpeeds>::Pack(
    google::protobuf::Message* msg, const frc::MecanumDriveWheelSpeeds& value) {
  auto m = static_cast<wpi::proto::ProtobufMecanumDriveWheelSpeeds*>(msg);
  m->set_front_left(value.frontLeft.value());
  m->set_front_right(value.frontRight.value());
  m->set_rear_left(value.rearLeft.value());
  m->set_rear_right(value.rearRight.value());
}
