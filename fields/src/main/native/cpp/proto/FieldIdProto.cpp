// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/proto/FieldIdProto.hpp"

#include "wpi/fields/protobuf/fields.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<wpi::fields::FieldId>
wpi::util::Protobuf<wpi::fields::FieldId>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose3d> pose;
  wpi_proto_ProtobufFieldId msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::fields::FieldId{msg.id};
}

bool wpi::util::Protobuf<wpi::fields::FieldId>::Pack(
    OutputStream& stream, const wpi::fields::FieldId& value) {
  wpi_proto_ProtobufFieldId msg{
      .id = std::to_underlying(value),
  };
  return stream.Encode(msg);
}
