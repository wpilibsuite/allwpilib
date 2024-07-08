// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/DifferentialDriveFeedforwardProto.h"

#include <wpi/ProtoHelper.h>

#include "controller.pb.h"

google::protobuf::Message* wpi::Protobuf<
    frc::DifferentialDriveFeedforward>::New(google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufDifferentialDriveFeedforward>(
      arena);
}

frc::DifferentialDriveFeedforward
wpi::Protobuf<frc::DifferentialDriveFeedforward>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufDifferentialDriveFeedforward*>(
      &msg);
  return {decltype(1_V / 1_mps){m->kv_linear()},
          decltype(1_V / 1_mps_sq){m->ka_linear()},
          decltype(1_V / 1_mps){m->kv_angular()},
          decltype(1_V / 1_mps_sq){m->ka_angular()}};
}

void wpi::Protobuf<frc::DifferentialDriveFeedforward>::Pack(
    google::protobuf::Message* msg,
    const frc::DifferentialDriveFeedforward& value) {
  auto m = static_cast<wpi::proto::ProtobufDifferentialDriveFeedforward*>(msg);
  m->set_kv_linear(value.m_kVLinear.value());
  m->set_ka_linear(value.m_kALinear.value());
  m->set_kv_angular(value.m_kVAngular.value());
  m->set_ka_angular(value.m_kAAngular.value());
}
