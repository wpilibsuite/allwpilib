// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/proto/MatrixProto.h"
#include "frc/system/LinearSystem.h"

template <int States, int Inputs, int Outputs>
struct wpi::Protobuf<frc::LinearSystem<States, Inputs, Outputs>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::LinearSystem<States, Inputs, Outputs> Unpack(
      const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg,
                   const frc::LinearSystem<States, Inputs, Outputs>& value);
};

#include "frc/system/proto/LinearSystemProto.inc"
