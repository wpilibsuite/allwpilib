// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Translation2d.h"
#include "geometry2d.pb.h"

using StructType = wpi::Struct<frc::Translation2d>;

frc::Translation2d StructType::Unpack(
    std::span<const uint8_t, StructType::kSize> data) {
  return {units::meter_t{wpi::UnpackStruct<double, 0>(data)},
          units::meter_t{wpi::UnpackStruct<double, 8>(data)}};
}
void StructType::Pack(std::span<uint8_t, StructType::kSize> data,
                      const frc::Translation2d& value) {
  wpi::PackStruct<0>(data, value.X().value());
  wpi::PackStruct<8>(data, value.Y().value());
}

google::protobuf::Message* wpi::Protobuf<frc::Translation2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTranslation2d>(arena);
}

frc::Translation2d wpi::Protobuf<frc::Translation2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTranslation2d*>(&msg);
  return frc::Translation2d{units::meter_t{m->x_meters()},
                            units::meter_t{m->y_meters()}};
}

void wpi::Protobuf<frc::Translation2d>::Pack(google::protobuf::Message* msg,
                                             const frc::Translation2d& value) {
  auto m = static_cast<wpi::proto::ProtobufTranslation2d*>(msg);
  m->set_x_meters(value.X().value());
  m->set_y_meters(value.Y().value());
}
