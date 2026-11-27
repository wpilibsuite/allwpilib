// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/proto/FieldTagProto.hpp"

#include "wpi/fields/protobuf/fields.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<wpi::fields::FieldTag>
wpi::util::Protobuf<wpi::fields::FieldTag>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose3d> pose;
  wpi_proto_ProtobufFieldTag msg{
      .id = 0,
      .pose = pose.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return wpi::fields::FieldTag{
      static_cast<int>(msg.id),
      ipose[0],
  };
}

bool wpi::util::Protobuf<wpi::fields::FieldTag>::Pack(
    OutputStream& stream, const wpi::fields::FieldTag& value) {
  wpi::util::PackCallback pose{&value.pose};
  wpi_proto_ProtobufFieldTag msg{
      .id = static_cast<uint32_t>(value.ID),
      .pose = pose.Callback(),
  };
  return stream.Encode(msg);
}
