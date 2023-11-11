// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Twist3d.h"
#include "geometry3d.pb.h"

using StructType = wpi::Struct<frc::Twist3d>;

frc::Twist3d StructType::Unpack(
    std::span<const uint8_t, StructType::kSize> data) {
  return {units::meter_t{wpi::UnpackStruct<double, 0>(data)},
          units::meter_t{wpi::UnpackStruct<double, 8>(data)},
          units::meter_t{wpi::UnpackStruct<double, 16>(data)},
          units::radian_t{wpi::UnpackStruct<double, 24>(data)},
          units::radian_t{wpi::UnpackStruct<double, 32>(data)},
          units::radian_t{wpi::UnpackStruct<double, 40>(data)}};
}
void StructType::Pack(std::span<uint8_t, StructType::kSize> data,
                      const frc::Twist3d& value) {
  wpi::PackStruct<0>(data, value.dx.value());
  wpi::PackStruct<8>(data, value.dy.value());
  wpi::PackStruct<16>(data, value.dz.value());
  wpi::PackStruct<24>(data, value.rx.value());
  wpi::PackStruct<32>(data, value.ry.value());
  wpi::PackStruct<40>(data, value.rz.value());
}

google::protobuf::Message* wpi::Protobuf<frc::Twist3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufTwist3d>(
      arena);
}

frc::Twist3d wpi::Protobuf<frc::Twist3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTwist3d*>(&msg);
  return frc::Twist3d{
      units::meter_t{m->dx_meters()},   units::meter_t{m->dy_meters()},
      units::meter_t{m->dz_meters()},   units::radian_t{m->rx_radians()},
      units::radian_t{m->ry_radians()}, units::radian_t{m->rz_radians()}};
}

void wpi::Protobuf<frc::Twist3d>::Pack(google::protobuf::Message* msg,
                                       const frc::Twist3d& value) {
  auto m = static_cast<wpi::proto::ProtobufTwist3d*>(msg);
  m->set_dx_meters(value.dx.value());
  m->set_dy_meters(value.dy.value());
  m->set_dz_meters(value.dz.value());
  m->set_rx_radians(value.rx.value());
  m->set_ry_radians(value.ry.value());
  m->set_rz_radians(value.rz.value());
}
