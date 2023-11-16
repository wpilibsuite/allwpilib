// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Twist3dProto.h"

#include "geometry3d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Twist3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufTwist3d>(
      arena);
}

frc::Twist3d wpi::Protobuf<frc::Twist3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTwist3d*>(&msg);
  return frc::Twist3d{
      units::meter_t{m->dx()},  units::meter_t{m->dy()},
      units::meter_t{m->dz()},  units::radian_t{m->rx()},
      units::radian_t{m->ry()}, units::radian_t{m->rz()},
  };
}

void wpi::Protobuf<frc::Twist3d>::Pack(google::protobuf::Message* msg,
                                       const frc::Twist3d& value) {
  auto m = static_cast<wpi::proto::ProtobufTwist3d*>(msg);
  m->set_dx(value.dx.value());
  m->set_dy(value.dy.value());
  m->set_dz(value.dz.value());
  m->set_rx(value.rx.value());
  m->set_ry(value.ry.value());
  m->set_rz(value.rz.value());
}
