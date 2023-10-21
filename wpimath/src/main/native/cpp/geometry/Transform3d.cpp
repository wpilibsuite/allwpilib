// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Transform3d.h"

#include "frc/geometry/Pose3d.h"
#include "geometry3d.pb.h"

using namespace frc;

Transform3d::Transform3d(Pose3d initial, Pose3d final) {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

Transform3d::Transform3d(Translation3d translation, Rotation3d rotation)
    : m_translation(std::move(translation)), m_rotation(std::move(rotation)) {}

Transform3d::Transform3d(units::meter_t x, units::meter_t y, units::meter_t z,
                         Rotation3d rotation)
    : m_translation(x, y, z), m_rotation(std::move(rotation)) {}

Transform3d Transform3d::Inverse() const {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  return Transform3d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
}

Transform3d Transform3d::operator+(const Transform3d& other) const {
  return Transform3d{Pose3d{}, Pose3d{}.TransformBy(*this).TransformBy(other)};
}

google::protobuf::Message* wpi::Protobuf<frc::Transform3d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTransform3d>(arena);
}

frc::Transform3d wpi::Protobuf<frc::Transform3d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTransform3d*>(&msg);
  return Transform3d{wpi::UnpackProtobuf<frc::Translation3d>(m->translation()),
                     wpi::UnpackProtobuf<frc::Rotation3d>(m->rotation())};
}

void wpi::Protobuf<frc::Transform3d>::Pack(google::protobuf::Message* msg,
                                           const frc::Transform3d& value) {
  auto m = static_cast<wpi::proto::ProtobufTransform3d*>(msg);
  wpi::PackProtobuf(m->mutable_translation(), value.Translation());
  wpi::PackProtobuf(m->mutable_rotation(), value.Rotation());
}
