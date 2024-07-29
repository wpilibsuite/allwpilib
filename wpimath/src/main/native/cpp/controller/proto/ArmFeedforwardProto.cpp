// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/ArmFeedforwardProto.h"

#include <wpi/ProtoHelper.h>

#include "controller.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::ArmFeedforward>::New(
    google::protobuf::Arena* arena) {
  return wpi::CreateMessage<wpi::proto::ProtobufArmFeedforward>(arena);
}

frc::ArmFeedforward wpi::Protobuf<frc::ArmFeedforward>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufArmFeedforward*>(&msg);
  return frc::ArmFeedforward{
      units::volt_t{m->ks()},
      units::volt_t{m->kg()},
      units::unit_t<frc::ArmFeedforward::kv_unit>{m->kv()},
      units::unit_t<frc::ArmFeedforward::ka_unit>{m->ka()},
  };
}

void wpi::Protobuf<frc::ArmFeedforward>::Pack(
    google::protobuf::Message* msg, const frc::ArmFeedforward& value) {
  auto m = static_cast<wpi::proto::ProtobufArmFeedforward*>(msg);
  m->set_ks(value.GetKs().value());
  m->set_kg(value.GetKg().value());
  m->set_kv(value.GetKv().value());
  m->set_ka(value.GetKa().value());
}
