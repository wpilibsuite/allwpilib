// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/FieldImage.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::fields;

namespace {

const FieldImage EXPECTED_DATA{"frc/2026-rebuilt.png", 121, 254, 1917, 3951};
}  // namespace

TEST_CASE("FieldImageProtoTest Roundtrip", "[fields]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  REQUIRE(unpacked_data->GetPath() == EXPECTED_DATA.GetPath());
  REQUIRE(unpacked_data->GetTop() == EXPECTED_DATA.GetTop());
  REQUIRE(unpacked_data->GetLeft() == EXPECTED_DATA.GetLeft());
  REQUIRE(unpacked_data->GetBottom() == EXPECTED_DATA.GetBottom());
  REQUIRE(unpacked_data->GetRight() == EXPECTED_DATA.GetRight());
}
