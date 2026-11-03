// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/Field.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::fields;

namespace {

const Field EXPECTED_DATA{
    "Test Field",
    "",
    "",
    {},
    54_ft,
    27_ft,
    "",
    std::vector{FieldTag{1, wpi::math::Pose3d{}},
                FieldTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                              wpi::math::Rotation3d{
                                                  0_deg, 0_deg, 0_deg}}}},
};
}  // namespace

TEST_CASE("FieldProtoTest Roundtrip", "[fields]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 256> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  REQUIRE(unpacked_data->GetLength() == EXPECTED_DATA.GetLength());
  REQUIRE(unpacked_data->GetWidth() == EXPECTED_DATA.GetWidth());

  REQUIRE(unpacked_data->GetTags() == EXPECTED_DATA.GetTags());
}
