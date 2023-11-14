// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/serde/Translation2dSerde.h"

#include "geometry2d.pb.h"

namespace {
constexpr size_t kXOff = 0;
constexpr size_t kYOff = kXOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::Translation2d>;

frc::Translation2d StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return frc::Translation2d{
      units::meter_t{wpi::UnpackStruct<double, kXOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kYOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::Translation2d& value) {
  wpi::PackStruct<kXOff>(data, value.X().value());
  wpi::PackStruct<kYOff>(data, value.Y().value());
}

google::protobuf::Message* wpi::Protobuf<frc::Translation2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTranslation2d>(arena);
}

frc::Translation2d wpi::Protobuf<frc::Translation2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTranslation2d*>(&msg);
  return frc::Translation2d{
      units::meter_t{m->x_meters()},
      units::meter_t{m->y_meters()},
  };
}

void wpi::Protobuf<frc::Translation2d>::Pack(google::protobuf::Message* msg,
                                             const frc::Translation2d& value) {
  auto m = static_cast<wpi::proto::ProtobufTranslation2d*>(msg);
  m->set_x_meters(value.X().value());
  m->set_y_meters(value.Y().value());
}
