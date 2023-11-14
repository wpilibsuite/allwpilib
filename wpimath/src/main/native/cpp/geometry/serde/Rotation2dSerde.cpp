// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/serde/Rotation2dSerde.h"

#include "geometry2d.pb.h"

using StructType = wpi::Struct<frc::Rotation2d>;

frc::Rotation2d StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return units::radian_t{wpi::UnpackStruct<double>(data)};
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::Rotation2d& value) {
  wpi::PackStruct(data, value.Radians().value());
}

google::protobuf::Message* wpi::Protobuf<frc::Rotation2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufRotation2d>(
      arena);
}

frc::Rotation2d wpi::Protobuf<frc::Rotation2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufRotation2d*>(&msg);
  return frc::Rotation2d{units::radian_t{m->radians()}};
}

void wpi::Protobuf<frc::Rotation2d>::Pack(google::protobuf::Message* msg,
                                          const frc::Rotation2d& value) {
  auto m = static_cast<wpi::proto::ProtobufRotation2d*>(msg);
  m->set_radians(value.Radians().value());
}
