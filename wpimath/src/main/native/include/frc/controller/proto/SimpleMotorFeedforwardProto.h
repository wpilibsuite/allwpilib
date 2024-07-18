// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/length.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters>

template <class Distance>
struct wpi::Protobuf<frc::SimpleMotorFeedforward<Distance>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::SimpleMotorFeedforward<Distance> Unpack(
      const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg,
                   const frc::SimpleMotorFeedforward<Distance>& value);
};

#include "frc/controller/proto/SimpleMotorFeedforwardProto.inc"
