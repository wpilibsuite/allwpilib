// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/proto/QuinticHermiteSplineProto.h"

#include <wpi/ProtoHelper.h>

#include "spline.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::QuinticHermiteSpline>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufQuinticHermiteSpline>(arena);
}

frc::QuinticHermiteSpline wpi::Protobuf<frc::QuinticHermiteSpline>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufQuinticHermiteSpline*>(&msg);
  return frc::QuinticHermiteSpline{
      wpi::UnpackProtobufArray<double, 3>(m->x_initial()),
      wpi::UnpackProtobufArray<double, 3>(m->x_final()),
      wpi::UnpackProtobufArray<double, 3>(m->y_initial()),
      wpi::UnpackProtobufArray<double, 3>(m->y_final())};
}

void wpi::Protobuf<frc::QuinticHermiteSpline>::Pack(
    google::protobuf::Message* msg, const frc::QuinticHermiteSpline& value) {
  auto m = static_cast<wpi::proto::ProtobufQuinticHermiteSpline*>(msg);
  wpi::PackProtobufArray(m->mutable_x_initial(),
                         value.GetInitialControlVector().x);
  wpi::PackProtobufArray(m->mutable_x_final(), value.GetFinalControlVector().x);
  wpi::PackProtobufArray(m->mutable_y_initial(),
                         value.GetInitialControlVector().y);
  wpi::PackProtobufArray(m->mutable_y_final(), value.GetFinalControlVector().y);
}
