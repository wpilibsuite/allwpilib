// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/ProtoHelper.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/EigenCore.h"
#include "wpimath.pb.h"

template <int Size, int Options, int MaxRows, int MaxCols>
struct wpi::Protobuf<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena) {
    return wpi::CreateMessage<wpi::proto::ProtobufVector>(arena);
  }

  static frc::Matrixd<Size, 1, Options, MaxRows, MaxCols> Unpack(
      const google::protobuf::Message& msg) {
    auto m = static_cast<const wpi::proto::ProtobufVector*>(&msg);
    if (m->rows_size() != Size) {
      throw std::invalid_argument(
          fmt::format("Tried to unpack message with {} elements in rows into "
                      "Vector with {} rows",
                      m->rows_size(), Size));
    }
    frc::Matrixd<Size, 1, Options, MaxRows, MaxCols> vec;
    for (int i = 0; i < Size; i++) {
      vec(i) = m->rows(i);
    }
    return vec;
  }

  static void Pack(
      google::protobuf::Message* msg,
      const frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value) {
    auto m = static_cast<wpi::proto::ProtobufVector*>(msg);
    m->clear_rows();
    for (int i = 0; i < Size; i++) {
      m->add_rows(value(i));
    }
  }
};
