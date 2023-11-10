// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/EigenCore.h"

template <int Size>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Vectord<Size>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Vectord<Size> Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg,
                   const frc::Vectord<Size>& value);
};

#include "frc/proto/VectorProto.inc"
