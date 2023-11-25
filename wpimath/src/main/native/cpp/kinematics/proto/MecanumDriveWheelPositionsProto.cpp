// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/MecanumDriveWheelPositionsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::MecanumDriveWheelPositions>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufMecanumDriveWheelPositions>(arena);
}

frc::MecanumDriveWheelPositions
wpi::Protobuf<frc::MecanumDriveWheelPositions>::Unpack(
    const google::protobuf::Message& msg) {
  auto m =
      static_cast<const wpi::proto::ProtobufMecanumDriveWheelPositions*>(&msg);
  return frc::MecanumDriveWheelPositions{
      units::meter_t{m->front_left()},
      units::meter_t{m->front_right()},
      units::meter_t{m->rear_left()},
      units::meter_t{m->rear_right()},
  };
}

void wpi::Protobuf<frc::MecanumDriveWheelPositions>::Pack(
    google::protobuf::Message* msg,
    const frc::MecanumDriveWheelPositions& value) {
  auto m = static_cast<wpi::proto::ProtobufMecanumDriveWheelPositions*>(msg);
  m->set_front_left(value.frontLeft.value());
  m->set_front_right(value.frontRight.value());
  m->set_rear_left(value.rearLeft.value());
  m->set_rear_right(value.rearRight.value());
}
