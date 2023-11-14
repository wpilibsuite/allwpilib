// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/serde/Translation3dSerde.h"

#include "geometry3d.pb.h"

namespace {
constexpr size_t kXOff = 0;
constexpr size_t kYOff = kXOff + 8;
constexpr size_t kZOff = kYOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::Translation3d>;

frc::Translation3d StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return frc::Translation3d{
      units::meter_t{wpi::UnpackStruct<double, kXOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kYOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kZOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::Translation3d& value) {
  wpi::PackStruct<kXOff>(data, value.X().value());
  wpi::PackStruct<kYOff>(data, value.Y().value());
  wpi::PackStruct<kZOff>(data, value.Z().value());
}

google::protobuf::Message* wpi::Protobuf<frc::Translation3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTranslation3d>(arena);
}

frc::Translation3d wpi::Protobuf<frc::Translation3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTranslation3d*>(&msg);
  return frc::Translation3d{
      units::meter_t{m->x_meters()},
      units::meter_t{m->y_meters()},
      units::meter_t{m->z_meters()},
  };
}

void wpi::Protobuf<frc::Translation3d>::Pack(google::protobuf::Message* msg,
                                             const frc::Translation3d& value) {
  auto m = static_cast<wpi::proto::ProtobufTranslation3d*>(msg);
  m->set_x_meters(value.X().value());
  m->set_y_meters(value.Y().value());
  m->set_z_meters(value.Z().value());
}
