// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/ArmFeedforwardProto.h"

#include "controller.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::ArmFeedforward>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufArmFeedforward>(arena);
}

frc::ArmFeedforward wpi::Protobuf<frc::ArmFeedforward>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufArmFeedforward*>(&msg);
  return frc::ArmFeedforward{
      m->ks(),
      m->kg(),
      m->kv(),
      m->ka(),
  };
}

void wpi::Protobuf<frc::ArmFeedforward>::Pack(
    google::protobuf::Message* msg, const frc::ArmFeedforward& value) {
  auto m = static_cast<wpi::proto::ProtobufArmFeedforward*>(msg);
  m->set_ks(value.ks.value());
  m->set_kg(value.kg.value());
  m->set_kv(value.kv.value());
  m->set_ka(value.ka.value());
}
