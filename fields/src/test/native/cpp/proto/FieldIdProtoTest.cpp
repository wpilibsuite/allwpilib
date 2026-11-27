// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/fields.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::fields;

namespace {

const FieldId EXPECTED_DATA = FieldId::DEFAULT_FIELD;
}  // namespace

TEST_CASE("FieldIdProtoTest Roundtrip", "[fields]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  REQUIRE(unpacked_data == EXPECTED_DATA);
}
