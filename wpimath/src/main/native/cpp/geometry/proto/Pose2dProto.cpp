// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/proto/Pose2dProto.h"

#include <wpi/ProtoHelper.h>

#include "geometry2d.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"

#include "geometry2d.npb.h"

std::optional<frc::Pose2d> wpi::Protobuf<frc::Pose2d>::Unpack(
    pb_istream_t& stream) {
  frc::Rotation2d rot;
  frc::Translation2d tsln;
  wpi_proto_ProtobufPose2d msg{
      .translation = wpi::UnpackCallback(tsln),
      .rotation = wpi::UnpackCallback(rot),

  };
  if (!pb_decode(stream, *get_wpi_proto_ProtobufPose2d_msg(), msg,
                 PB_DECODE_NOINIT)) {
    return {};
  }

  return frc::Pose2d{tsln, rot};
}

bool wpi::Protobuf<frc::Pose2d>::Pack(pb_ostream_t& stream,
                                      const frc::Pose2d& value) {
  wpi_proto_ProtobufPose2d msg{
      .translation = wpi::PackCallback(value.Translation()),
      .rotation = wpi::PackCallback(value.Rotation()),

  };
  return pb_encode(stream, *get_wpi_proto_ProtobufPose2d_msg(), msg);
}

google::protobuf::Message* wpi::Protobuf<frc::Pose2d>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufPose2d>(arena);
}

frc::Pose2d wpi::Protobuf<frc::Pose2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufPose2d*>(&msg);
  return frc::Pose2d{
      wpi::UnpackProtobuf<frc::Translation2d>(m->wpi_translation()),
      wpi::UnpackProtobuf<frc::Rotation2d>(m->wpi_rotation()),
  };
}

void wpi::Protobuf<frc::Pose2d>::Pack(google::protobuf::Message* msg,
                                      const frc::Pose2d& value) {
  auto m = static_cast<wpi::proto::ProtobufPose2d*>(msg);
  wpi::PackProtobuf(m->mutable_translation(), value.Translation());
  wpi::PackProtobuf(m->mutable_rotation(), value.Rotation());
}
