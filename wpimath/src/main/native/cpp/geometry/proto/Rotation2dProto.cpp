// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Rotation2dProto.h"

#include <wpi/ProtoHelper.h>

#include "geometry2d.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"

#include "geometry2d.npb.h"

std::optional<frc::Rotation2d> wpi::Protobuf<frc::Rotation2d>::Unpack(
    pb_istream_t& stream) {
  wpi_proto_ProtobufRotation2d msg;
  if (!pb_decode(stream, *get_wpi_proto_ProtobufRotation2d_msg(), msg,
                 PB_DECODE_NOINIT)) {
    return {};
  }

  return frc::Rotation2d{
      units::radian_t{msg.value},
  };
}

bool wpi::Protobuf<frc::Rotation2d>::Pack(pb_ostream_t& stream,
                                          const frc::Rotation2d& value) {
  wpi_proto_ProtobufRotation2d msg {
    .value = value.Radians().value(),
  };
  return pb_encode(stream, *get_wpi_proto_ProtobufRotation2d_msg(), msg);
}

google::protobuf::Message* wpi::Protobuf<frc::Rotation2d>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufRotation2d>(arena);
}

frc::Rotation2d wpi::Protobuf<frc::Rotation2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufRotation2d*>(&msg);
  return frc::Rotation2d{
      units::radian_t{m->value()},
  };
}

void wpi::Protobuf<frc::Rotation2d>::Pack(google::protobuf::Message* msg,
                                          const frc::Rotation2d& value) {
  auto m = static_cast<wpi::proto::ProtobufRotation2d*>(msg);
  m->set_value(value.Radians().value());
}
