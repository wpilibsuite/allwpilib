// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include <string>

#include "wpiutil.npb.h"

struct TestProtoInner {
  std::string msg;
};

template <>
struct wpi::Protobuf<TestProtoInner> {
  using MessageStruct = wpi_proto_TestProtoInner;
  using InputStream = wpi::ProtoInputStream<TestProtoInner>;
  using OutputStream = wpi::ProtoOutputStream<TestProtoInner>;
  static std::optional<TestProtoInner> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const TestProtoInner& value);
};
