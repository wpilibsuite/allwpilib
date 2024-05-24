// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveWheelPositionsProto.h"

#include "kinematics.pb.h"

google::protobuf::Message* wpi::Protobuf<
    frc::DifferentialDriveWheelPositions>::New(google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufDifferentialDriveWheelPositions>(arena);
}

frc::DifferentialDriveWheelPositions
wpi::Protobuf<frc::DifferentialDriveWheelPositions>::Unpack(
    const google::protobuf::Message& msg) {
  auto m =
      static_cast<const wpi::proto::ProtobufDifferentialDriveWheelPositions*>(
          &msg);
  return frc::DifferentialDriveWheelPositions{
      units::meter_t{m->left()},
      units::meter_t{m->right()},
  };
}

void wpi::Protobuf<frc::DifferentialDriveWheelPositions>::Pack(
    google::protobuf::Message* msg,
    const frc::DifferentialDriveWheelPositions& value) {
  auto m =
      static_cast<wpi::proto::ProtobufDifferentialDriveWheelPositions*>(msg);
  m->set_left(value.left.value());
  m->set_right(value.right.value());
}
