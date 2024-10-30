// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Translation2dProto.h"

#include <wpi/ProtoHelper.h>

#include "geometry2d.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"

#include "geometry2d.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Translation2d>::Message() {
  return get_wpi_proto_ProtobufTranslation2d_msg();
}

std::optional<frc::Translation2d> wpi::Protobuf<frc::Translation2d>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufTranslation2d msg;
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  return frc::Translation2d{
      units::meter_t{msg.x},
      units::meter_t{msg.y},
  };
}

bool wpi::Protobuf<frc::Translation2d>::Pack(wpi::ProtoOutputStream& stream,
                                             const frc::Translation2d& value) {
  wpi_proto_ProtobufTranslation2d msg{
      .x = value.X().value(),
      .y = value.Y().value(),
  };
  return stream.Encode(msg);
}

google::protobuf::Message* wpi::Protobuf<frc::Translation2d>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufTranslation2d>(arena);
}

frc::Translation2d wpi::Protobuf<frc::Translation2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTranslation2d*>(&msg);
  return frc::Translation2d{
      units::meter_t{m->x()},
      units::meter_t{m->y()},
  };
}

void wpi::Protobuf<frc::Translation2d>::Pack(google::protobuf::Message* msg,
                                             const frc::Translation2d& value) {
  auto m = static_cast<wpi::proto::ProtobufTranslation2d*>(msg);
  m->set_x(value.X().value());
  m->set_y(value.Y().value());
}
