// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/Mechanism2d.h"
#include "frc/smartdashboard/MechanismLigament2d.h"
#include "frc/smartdashboard/proto/Mechanism2dProto.h"
#include <wpi/protobuf/Protobuf.h>
#include "mechanism2d.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Mechanism2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufMechanism2d>(arena);
}

frc::Mechanism2d wpi::Protobuf<frc::Mechanism2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufMechanism2d*>(&msg);
  auto mechanism = frc::Mechanism2d{m->width(), m->height(), frc::Color8Bit{m->background_color()}};
  for (auto root : m->roots()) {
    auto mechanismRoot = mechanism.GetRoot(root.name(), root.x(), root.y());
    for (auto ligament: root.ligaments()) {
      mechanismRoot->Append<frc::MechanismLigament2d>(ligament.name(), ligament.length(), units::angle::degree_t{ligament.angle()}, ligament.weight(), frc::Color8Bit{ligament.color()});
    }
  }
}

void wpi::Protobuf<frc::Mechanism2d>::Pack(
    google::protobuf::Message* msg, const frc::Mechanism2d& value) {
  auto m = static_cast<wpi::proto::ProtobufMechanism2d*>(msg);
  m->set_background_color(value.GetBackgroundColor());
  m->set_height(value.GetHeight());
  m->set_width(value.GetWidth());
  for (auto&& root : value.GetAllRoots()) {
    // auto mechanismRoot = ;
    wpi::PackProtobuf<frc::MechanismRoot2d>(m->add_roots(), frc::MechanismRoot2d{"",1,1, frc::MechanismRoot2d::private_init{}}); 
  }
}
