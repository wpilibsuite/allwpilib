// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Rotation2d.h"

#include <cmath>

#include <wpi/json.h>

#include "geometry2d.pb.h"
#include "units/math.h"

using namespace frc;

void frc::to_json(wpi::json& json, const Rotation2d& rotation) {
  json = wpi::json{{"radians", rotation.Radians().value()}};
}

void frc::from_json(const wpi::json& json, Rotation2d& rotation) {
  rotation = Rotation2d{units::radian_t{json.at("radians").get<double>()}};
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
