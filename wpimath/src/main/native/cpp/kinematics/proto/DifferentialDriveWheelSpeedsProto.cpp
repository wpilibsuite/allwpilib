// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveWheelSpeedsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<
    frc::DifferentialDriveWheelSpeeds>::New(google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufDifferentialDriveWheelSpeeds>(arena);
}

frc::DifferentialDriveWheelSpeeds
wpi::Protobuf<frc::DifferentialDriveWheelSpeeds>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufDifferentialDriveWheelSpeeds*>(
      &msg);
  return frc::DifferentialDriveWheelSpeeds{
      units::meters_per_second_t{m->left()},
      units::meters_per_second_t{m->right()},
  };
}

void wpi::Protobuf<frc::DifferentialDriveWheelSpeeds>::Pack(
    google::protobuf::Message* msg,
    const frc::DifferentialDriveWheelSpeeds& value) {
  auto m = static_cast<wpi::proto::ProtobufDifferentialDriveWheelSpeeds*>(msg);
  m->set_left(value.left.value());
  m->set_right(value.right.value());
}
