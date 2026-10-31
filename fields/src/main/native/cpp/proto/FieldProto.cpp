// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/proto/FieldProto.hpp"

#include <string>
#include <vector>

#include "wpi/fields/protobuf/fields.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

struct FieldDimensions {
  wpi::units::meter_t length;
  wpi::units::meter_t width;
};

template <>
struct wpi::util::Protobuf<FieldDimensions> {
  using MessageStruct = wpi_proto_ProtobufField_ProtobufFieldDimensions;
  using InputStream = wpi::util::ProtoInputStream<FieldDimensions>;
  using OutputStream = wpi::util::ProtoOutputStream<FieldDimensions>;
  static std::optional<FieldDimensions> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const FieldDimensions& value);
};

std::optional<wpi::fields::Field>
wpi::util::Protobuf<wpi::fields::Field>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<std::string> name;
  wpi::util::UnpackCallback<std::string> season;
  wpi::util::UnpackCallback<std::string> game;
  wpi::util::UnpackCallback<FieldDimensions> field_dimensions;
  wpi::util::UnpackCallback<std::string> program;
  wpi::util::UnpackCallback<wpi::fields::FieldImage> image;
  wpi::util::StdVectorUnpackCallback<wpi::fields::FieldTag, SIZE_MAX> tags;
  wpi_proto_ProtobufField msg{
      .name = name.Callback(),
      .season = season.Callback(),
      .game = game.Callback(),
      .field_dimensions = field_dimensions.Callback(),
      .program = program.Callback(),
      .image = image.Callback(),
      .tags = tags.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iname = name.Items();
  auto iseason = season.Items();
  auto igame = game.Items();
  auto ifield_dimensions = field_dimensions.Items();
  auto iprogram = program.Items();
  auto iimage = image.Items();

  if (iname.empty() || iseason.empty() || igame.empty() ||
      ifield_dimensions.empty() || iprogram.empty()) {
    return {};
  }

  return wpi::fields::Field{
      iname[0],
      iseason[0],
      igame[0],
      iimage.empty() ? std::nullopt : std::optional{iimage[0]},
      ifield_dimensions[0].length,
      ifield_dimensions[0].width,
      iprogram[0],
      tags.Vec(),
  };
}

bool wpi::util::Protobuf<wpi::fields::Field>::Pack(
    OutputStream& stream, const wpi::fields::Field& value) {
  wpi::util::PackCallback name{&value.m_name};
  wpi::util::PackCallback season{&value.m_season};
  wpi::util::PackCallback game{&value.m_game};
  FieldDimensions fieldDimensions = {value.m_fieldLength, value.m_fieldWidth};
  wpi::util::PackCallback field_dimensions{&fieldDimensions};
  wpi::util::PackCallback program{&value.m_program};
  // GetTags returns a temporary vector that would get discarded if passed
  // directly to the pack callback
  wpi::util::PackCallback tags{value.m_tags};
  wpi_proto_ProtobufField msg{
      .name = name.Callback(),
      .season = season.Callback(),
      .game = game.Callback(),
      .field_dimensions = field_dimensions.Callback(),
      .program = program.Callback(),
      .image = {},
      .tags = tags.Callback(),
  };

  std::optional<wpi::fields::FieldImage> field_image = value.GetImage();
  if (field_image.has_value()) {
    // FIXME: Don't discard image before encode is called.
    wpi::util::PackCallback<wpi::fields::FieldImage> image{
        &field_image.value()};
    msg.image = image.Callback();
  }

  return stream.Encode(msg);
}

std::optional<FieldDimensions> wpi::util::Protobuf<FieldDimensions>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufField_ProtobufFieldDimensions msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return FieldDimensions{
      units::meter_t{msg.length},
      units::meter_t{msg.width},
  };
}

bool wpi::util::Protobuf<FieldDimensions>::Pack(OutputStream& stream,
                                                const FieldDimensions& value) {
  wpi_proto_ProtobufField_ProtobufFieldDimensions msg{
      .length = value.length.value(),
      .width = value.width.value(),
  };
  return stream.Encode(msg);
}
