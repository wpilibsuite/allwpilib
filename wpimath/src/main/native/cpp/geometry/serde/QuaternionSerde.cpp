// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/serde/QuaternionSerde.h"

#include "geometry3d.pb.h"

namespace {
constexpr size_t kWOff = 0;
constexpr size_t kXOff = kWOff + 8;
constexpr size_t kYOff = kXOff + 8;
constexpr size_t kZOff = kYOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::Quaternion>;

frc::Quaternion StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return frc::Quaternion{
      wpi::UnpackStruct<double, kWOff>(data),
      wpi::UnpackStruct<double, kXOff>(data),
      wpi::UnpackStruct<double, kYOff>(data),
      wpi::UnpackStruct<double, kZOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::Quaternion& value) {
  wpi::PackStruct<kWOff>(data, value.W());
  wpi::PackStruct<kXOff>(data, value.X());
  wpi::PackStruct<kYOff>(data, value.Y());
  wpi::PackStruct<kZOff>(data, value.Z());
}

google::protobuf::Message* wpi::Protobuf<frc::Quaternion>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufQuaternion>(
      arena);
}

frc::Quaternion wpi::Protobuf<frc::Quaternion>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufQuaternion*>(&msg);
  return frc::Quaternion{
      m->w(),
      m->x(),
      m->y(),
      m->z(),
  };
}

void wpi::Protobuf<frc::Quaternion>::Pack(google::protobuf::Message* msg,
                                          const frc::Quaternion& value) {
  auto m = static_cast<wpi::proto::ProtobufQuaternion*>(msg);
  m->set_w(value.W());
  m->set_x(value.X());
  m->set_y(value.Y());
  m->set_z(value.Z());
}
