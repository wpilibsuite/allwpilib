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

template <int Rows, int Cols, int Options, int MaxRows, int MaxCols>
  requires(Cols != 1)
struct wpi::Protobuf<frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena) {
    return wpi::CreateMessage<wpi::proto::ProtobufMatrix>(arena);
  }

  static frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> Unpack(
      const google::protobuf::Message& msg) {
    auto m = static_cast<const wpi::proto::ProtobufMatrix*>(&msg);
    if (m->num_rows() != Rows || m->num_cols() != Cols) {
      throw std::invalid_argument(fmt::format(
          "Tried to unpack message with {} rows and {} columns into "
          "Matrix with {} rows and {} columns",
          m->num_rows(), m->num_cols(), Rows, Cols));
    }
    if (m->data_size() != Rows * Cols) {
      throw std::invalid_argument(
          fmt::format("Tried to unpack message with {} elements in data into "
                      "Matrix with {} elements",
                      m->data_size(), Rows * Cols));
    }
    frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> mat;
    for (int i = 0; i < Rows * Cols; i++) {
      mat(i) = m->data(i);
    }
    return mat;
  }

  static void Pack(
      google::protobuf::Message* msg,
      const frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>& value) {
    auto m = static_cast<wpi::proto::ProtobufMatrix*>(msg);
    m->set_num_rows(Rows);
    m->set_num_cols(Cols);
    m->clear_data();
    for (int i = 0; i < Rows * Cols; i++) {
      m->add_data(value(i));
    }
  }
};
