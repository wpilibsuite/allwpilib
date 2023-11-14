// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Twist2d.h"

#include "geometry2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Twist2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufTwist2d>(
      arena);
}

frc::Twist2d wpi::Protobuf<frc::Twist2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTwist2d*>(&msg);
  return frc::Twist2d{
      units::meter_t{m->dx_meters()},
      units::meter_t{m->dy_meters()},
      units::radian_t{m->dtheta_radians()},
  };
}

void wpi::Protobuf<frc::Twist2d>::Pack(google::protobuf::Message* msg,
                                       const frc::Twist2d& value) {
  auto m = static_cast<wpi::proto::ProtobufTwist2d*>(msg);
  m->set_dx_meters(value.dx.value());
  m->set_dy_meters(value.dy.value());
  m->set_dtheta_radians(value.dtheta.value());
}
