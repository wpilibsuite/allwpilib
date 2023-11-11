// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Twist2d.h"
#include "geometry2d.pb.h"

using StructType = wpi::Struct<frc::Twist2d>;

frc::Twist2d StructType::Unpack(
    std::span<const uint8_t, StructType::kSize> data) {
  return {units::meter_t{wpi::UnpackStruct<double, 0>(data)},
          units::meter_t{wpi::UnpackStruct<double, 8>(data)},
          units::radian_t{wpi::UnpackStruct<double, 16>(data)}};
}
void StructType::Pack(std::span<uint8_t, StructType::kSize> data,
                      const frc::Twist2d& value) {
  wpi::PackStruct<0>(data, value.dx.value());
  wpi::PackStruct<8>(data, value.dy.value());
  wpi::PackStruct<16>(data, value.dtheta.value());
}

google::protobuf::Message* wpi::Protobuf<frc::Twist2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufTwist2d>(
      arena);
}

frc::Twist2d wpi::Protobuf<frc::Twist2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTwist2d*>(&msg);
  return frc::Twist2d{units::meter_t{m->dx_meters()},
                      units::meter_t{m->dy_meters()},
                      units::radian_t{m->dtheta_radians()}};
}

void wpi::Protobuf<frc::Twist2d>::Pack(google::protobuf::Message* msg,
                                       const frc::Twist2d& value) {
  auto m = static_cast<wpi::proto::ProtobufTwist2d*>(msg);
  m->set_dx_meters(value.dx.value());
  m->set_dy_meters(value.dy.value());
  m->set_dtheta_radians(value.dtheta.value());
}
