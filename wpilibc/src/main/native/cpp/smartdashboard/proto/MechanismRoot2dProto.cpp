// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismRoot2d.h"
#include "frc/smartdashboard/MechanismLigament2d.h"
#include "frc/smartdashboard/proto/MechanismRoot2dProto.h"

#include "mechanism2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::MechanismRoot2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufMechanismRoot2d>(arena);
}

frc::MechanismRoot2d wpi::Protobuf<frc::MechanismRoot2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufMechanismRoot2d*>(&msg);
  auto root = frc::MechanismRoot2d{m->name(), m->x(), m->y()};
  for (const auto ligament : m->ligaments()) {
    root.Append<frc::MechanismLigament2d>(ligament.name(), wpi::UnpackProtobuf<frc::MechanismLigament2d>(ligament));
  }
}

void wpi::Protobuf<frc::MechanismRoot2d>::Pack(
    google::protobuf::Message* msg, const frc::MechanismRoot2d& value) {
  auto m = static_cast<wpi::proto::ProtobufMechanismRoot2d*>(msg);
  m->set_name(value.GetName());
  m->set_x(value.GetX());
  m->set_y(value.GetY());
  for (auto&& root : value.GetObjects()) {
    // auto mechanismRoot = ;
    wpi::PackProtobuf<frc::MechanismRoot2d>(m->add_ligaments(), *root.second); 
  }
}
