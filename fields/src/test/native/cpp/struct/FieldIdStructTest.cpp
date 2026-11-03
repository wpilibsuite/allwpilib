// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/fields.hpp"

using namespace wpi::fields;

namespace {

using StructType = wpi::util::Struct<wpi::fields::FieldId>;
const FieldId EXPECTED_DATA = FieldId::DEFAULT_FIELD;

}  // namespace

TEST_CASE("FieldIdStructTest Roundtrip", "[fields]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  FieldId unpacked_data = StructType::Unpack(buffer);

  REQUIRE(unpacked_data == EXPECTED_DATA);
}
