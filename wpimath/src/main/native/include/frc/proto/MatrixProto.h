// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/EigenCore.h"

template <int Rows, int Cols, int Options, int MaxRows, int MaxCols>
  requires(Cols != 1)
struct wpi::Protobuf<frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> Unpack(
      const google::protobuf::Message& msg);
  static void Pack(
      google::protobuf::Message* msg,
      const frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>& value);
};

#include "frc/proto/MatrixProto.inc"
