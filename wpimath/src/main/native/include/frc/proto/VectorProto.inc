// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/proto/VectorProto.h"
#include "wpimath.pb.h"

template <int Size>
google::protobuf::Message* wpi::Protobuf<frc::Vectord<Size>>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufVector>(
      arena);
}

template <int Size>
frc::Vectord<Size> wpi::Protobuf<frc::Vectord<Size>>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufVector*>(&msg);
  if (m->rows_size() != Size) {
    // TODO Error
  }
  frc::Vectord<Size> vec;
  for (int i = 0; i < Size; i++) {
    vec(i) = m->rows(i);
  }
  return vec;
}

template <int Size>
void wpi::Protobuf<frc::Vectord<Size>>::Pack(google::protobuf::Message* msg,
                                             const frc::Vectord<Size>& value) {
  auto m = static_cast<wpi::proto::ProtobufVector*>(msg);
  m->clear_rows();
  for (int i = 0; i < Size; i++) {
    m->add_rows(value(i));
  }
}
