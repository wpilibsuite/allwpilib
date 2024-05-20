// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/DifferentialDriveWheelVoltagesProto.h"

#include "controller.pb.h"

google::protobuf::Message* wpi::Protobuf<
    frc::DifferentialDriveWheelVoltages>::New(google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufDifferentialDriveWheelVoltages>(arena);
}

frc::DifferentialDriveWheelVoltages
wpi::Protobuf<frc::DifferentialDriveWheelVoltages>::Unpack(
    const google::protobuf::Message& msg) {
  auto m =
      static_cast<const wpi::proto::ProtobufDifferentialDriveWheelVoltages*>(
          &msg);
  return frc::DifferentialDriveWheelVoltages{
      units::volt_t{m->left()},
      units::volt_t{m->right()},
  };
}

void wpi::Protobuf<frc::DifferentialDriveWheelVoltages>::Pack(
    google::protobuf::Message* msg,
    const frc::DifferentialDriveWheelVoltages& value) {
  auto m =
      static_cast<wpi::proto::ProtobufDifferentialDriveWheelVoltages*>(msg);
  m->set_left(value.left.value());
  m->set_right(value.right.value());
}
