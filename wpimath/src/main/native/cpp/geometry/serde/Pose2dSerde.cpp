// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/serde/Pose2dSerde.h"

#include "geometry2d.pb.h"

namespace {
constexpr size_t kTranslationOff = 0;
constexpr size_t kRotationOff =
    kTranslationOff + wpi::Struct<frc::Translation2d>::kSize;
}  // namespace

using StructType = wpi::Struct<frc::Pose2d>;

frc::Pose2d StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return frc::Pose2d{
      wpi::UnpackStruct<frc::Translation2d, kTranslationOff>(data),
      wpi::UnpackStruct<frc::Rotation2d, kRotationOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::Pose2d& value) {
  wpi::PackStruct<kTranslationOff>(data, value.Translation());
  wpi::PackStruct<kRotationOff>(data, value.Rotation());
}

void StructType::ForEachNested(
    std::invocable<std::string_view, std::string_view> auto fn) {
  wpi::ForEachStructSchema<frc::Translation2d>(fn);
  wpi::ForEachStructSchema<frc::Rotation2d>(fn);
}

google::protobuf::Message* wpi::Protobuf<frc::Pose2d>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufPose2d>(
      arena);
}

frc::Pose2d wpi::Protobuf<frc::Pose2d>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufPose2d*>(&msg);
  return frc::Pose2d{
      wpi::UnpackProtobuf<frc::Translation2d>(m->translation()),
      wpi::UnpackProtobuf<frc::Rotation2d>(m->rotation()),
  };
}

void wpi::Protobuf<frc::Pose2d>::Pack(google::protobuf::Message* msg,
                                      const frc::Pose2d& value) {
  auto m = static_cast<wpi::proto::ProtobufPose2d*>(msg);
  wpi::PackProtobuf(m->mutable_translation(), value.Translation());
  wpi::PackProtobuf(m->mutable_rotation(), value.Rotation());
}
