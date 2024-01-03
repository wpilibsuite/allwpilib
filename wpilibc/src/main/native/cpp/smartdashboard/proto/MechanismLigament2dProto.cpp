// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismLigament2d.h"
#include "frc/smartdashboard/proto/MechanismLigament2dProto.h"

#include "mechanism2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::MechanismLigament2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufMechanismLigament2d>(arena);
}

frc::MechanismLigament2d wpi::Protobuf<frc::MechanismLigament2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufMechanismLigament2d*>(&msg);
  auto root = frc::MechanismLigament2d{m->name(), m->length(), units::angle::degree_t{m->angle()}, m->weight(), frc::Color8Bit{m->color()}};
  for (const auto ligament : m->ligaments()) {
    root.Append<frc::MechanismLigament2d>(wpi::UnpackProtobuf<frc::MechanismLigament2d>(ligament));
  }
}

void wpi::Protobuf<frc::MechanismLigament2d>::Pack(
    google::protobuf::Message* msg, const frc::MechanismLigament2d& value) {
  auto m = static_cast<wpi::proto::ProtobufMechanismLigament2d*>(msg);
  m->set_name(value.GetName());
  m->set_length(value.GetLengthConst());
  m->set_angle(value.GetAngleConst());
  m->set_color(value.GetColorConst());
  m->set_weight(value.GetLineWeightConst());
  for (auto& root : value.GetObjects()) {
    // auto mechanismRoot = ;
    wpi::PackProtobuf<frc::MechanismLigament2d>(m->add_ligaments(), *root.second); 
  }
}
