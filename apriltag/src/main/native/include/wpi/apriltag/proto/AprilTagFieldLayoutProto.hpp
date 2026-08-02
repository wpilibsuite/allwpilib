// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "apriltag/protobuf/apriltag.npb.h"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct WPILIB_DLLEXPORT
    wpi::util::Protobuf<wpi::apriltag::AprilTagFieldLayout> {
  using MessageStruct = wpi_proto_ProtobufAprilTagFieldLayout;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::apriltag::AprilTagFieldLayout>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::apriltag::AprilTagFieldLayout>;
  static std::optional<wpi::apriltag::AprilTagFieldLayout> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::apriltag::AprilTagFieldLayout& value);
};

template <>
struct wpi::util::Protobuf<
    wpi::apriltag::AprilTagFieldLayout::FieldDimensions> {
  using MessageStruct =
      wpi_proto_ProtobufAprilTagFieldLayout_ProtobufFieldDimensions;
  using InputStream = wpi::util::ProtoInputStream<
      wpi::apriltag::AprilTagFieldLayout::FieldDimensions>;
  using OutputStream = wpi::util::ProtoOutputStream<
      wpi::apriltag::AprilTagFieldLayout::FieldDimensions>;
  static std::optional<wpi::apriltag::AprilTagFieldLayout::FieldDimensions>
  Unpack(InputStream& stream);
  static bool Pack(
      OutputStream& stream,
      const wpi::apriltag::AprilTagFieldLayout::FieldDimensions& value);
};
