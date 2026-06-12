// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/proto/AprilTagFieldLayoutProto.hpp"

#include <vector>

#include "apriltag/protobuf/apriltag.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<wpi::apriltag::AprilTagFieldLayout> wpi::util::Protobuf<
    wpi::apriltag::AprilTagFieldLayout>::Unpack(InputStream& stream) {
  wpi::util::StdVectorUnpackCallback<wpi::apriltag::AprilTag, SIZE_MAX> tags;
  wpi::util::UnpackCallback<wpi::apriltag::AprilTagFieldLayout::FieldDimensions>
      field;
  wpi_proto_ProtobufAprilTagFieldLayout msg{
      .tags = tags.Callback(),
      .field = field.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ifield = field.Items();

  if (ifield.empty()) {
    return {};
  }

  return wpi::apriltag::AprilTagFieldLayout{
      tags.Vec(),
      ifield[0].length,
      ifield[0].width,
  };
}

bool wpi::util::Protobuf<wpi::apriltag::AprilTagFieldLayout>::Pack(
    OutputStream& stream, const wpi::apriltag::AprilTagFieldLayout& value) {
  // GetTags returns a temporary vector that would get discarded if passed
  // directly to the pack callback
  std::vector<wpi::apriltag::AprilTag> tagVec = value.GetTags();
  wpi::util::PackCallback<wpi::apriltag::AprilTag> tags{tagVec};
  wpi::util::PackCallback<wpi::apriltag::AprilTagFieldLayout::FieldDimensions>
      field{&value.m_field};
  wpi_proto_ProtobufAprilTagFieldLayout msg{
      .tags = tags.Callback(),
      .field = field.Callback(),
  };
  return stream.Encode(msg);
}

std::optional<wpi::apriltag::AprilTagFieldLayout::FieldDimensions> wpi::util::
    Protobuf<wpi::apriltag::AprilTagFieldLayout::FieldDimensions>::Unpack(
        InputStream& stream) {
  wpi_proto_ProtobufAprilTagFieldLayout_ProtobufFieldDimensions msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::apriltag::AprilTagFieldLayout::FieldDimensions{
      units::meter_t{msg.length},
      units::meter_t{msg.width},
  };
}

bool wpi::util::Protobuf<wpi::apriltag::AprilTagFieldLayout::FieldDimensions>::
    Pack(OutputStream& stream,
         const wpi::apriltag::AprilTagFieldLayout::FieldDimensions& value) {
  wpi_proto_ProtobufAprilTagFieldLayout_ProtobufFieldDimensions msg{
      .length = value.length.value(),
      .width = value.width.value(),
  };
  return stream.Encode(msg);
}
