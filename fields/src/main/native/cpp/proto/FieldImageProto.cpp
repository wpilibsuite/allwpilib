// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/proto/FieldImageProto.hpp"

#include <string>

#include "wpi/fields/protobuf/fields.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<wpi::fields::FieldImage>
wpi::util::Protobuf<wpi::fields::FieldImage>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<std::string> path;
  wpi_proto_ProtobufFieldImage msg{
      .path = path.Callback(),
      .top = 0,
      .left = 0,
      .bottom = 0,
      .right = 0,
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ipath = path.Items();

  if (ipath.empty()) {
    return {};
  }

  return wpi::fields::FieldImage{
      ipath[0],
      static_cast<int>(msg.top),
      static_cast<int>(msg.left),
      static_cast<int>(msg.bottom),
      static_cast<int>(msg.right),
  };
}

bool wpi::util::Protobuf<wpi::fields::FieldImage>::Pack(
    OutputStream& stream, const wpi::fields::FieldImage& value) {
  wpi::util::PackCallback path{&value.m_path};
  wpi_proto_ProtobufFieldImage msg{
      .path = path.Callback(),
      .top = static_cast<uint32_t>(value.GetTop()),
      .left = static_cast<uint32_t>(value.GetLeft()),
      .bottom = static_cast<uint32_t>(value.GetBottom()),
      .right = static_cast<uint32_t>(value.GetRight()),
  };
  return stream.Encode(msg);
}
