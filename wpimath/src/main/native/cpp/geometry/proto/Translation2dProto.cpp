// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Translation2dProto.h"

#include <wpi/ProtoHelper.h>

#include "geometry2d.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"

#include "geometry2d.npb.h"

std::optional<frc::Translation2d> wpi::Protobuf<frc::Translation2d>::Unpack(
    pb_istream_t& stream) {
  wpi_proto_ProtobufTranslation2d msg;
  if (!pb_decode(stream, *get_wpi_proto_ProtobufTranslation2d_msg(), msg,
                 PB_DECODE_NOINIT)) {
    return {};
  }

  return frc::Translation2d{
      units::meter_t{msg.x},
      units::meter_t{msg.y},
  };
}

bool wpi::Protobuf<frc::Translation2d>::Pack(pb_ostream_t& stream,
                                             const frc::Translation2d& value, bool is_subobject) {
  wpi_proto_ProtobufTranslation2d msg{
      .x = value.X().value(),
      .y = value.Y().value(),
  };
  if (is_subobject) {
    return pb_encode_submessage(stream, *get_wpi_proto_ProtobufTranslation2d_msg(), msg);
  } else {
    return pb_encode(stream, *get_wpi_proto_ProtobufTranslation2d_msg(), msg);
  }
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
