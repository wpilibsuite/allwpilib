// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "pb.h"

struct TestProtoInner {
    std::string msg;
};

template <>
struct wpi::Protobuf<TestProtoInner> {
  static const pb_msgdesc_t* Message();
  static std::optional<TestProtoInner> Unpack(wpi::ProtoInputStream& stream);
  static bool Pack(wpi::ProtoOutputStream& stream, const TestProtoInner& value);
};
