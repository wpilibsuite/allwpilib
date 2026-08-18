// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/fields.hpp"

namespace wpi::fields {

static constexpr FieldId ALL_FIELDS[] = {
    FieldId::FRC_2018_POWER_UP,
    FieldId::FRC_2019_DEEP_SPACE,
    FieldId::FRC_2020_INFINITE_RECHARGE,
    FieldId::FRC_2021_INFINITE_RECHARGE,
    FieldId::FRC_2021_BARREL,
    FieldId::FRC_2021_BOUNCE,
    FieldId::FRC_2021_GALACTIC_SEARCH_A,
    FieldId::FRC_2021_GALACTIC_SEARCH_B,
    FieldId::FRC_2021_SLALOM,
    FieldId::FRC_2022_RAPID_REACT,
    FieldId::FRC_2023_CHARGED_UP,
    FieldId::FRC_2024_CRESCENDO,
    FieldId::FTC_2024_2025_INTO_THE_DEEP,
    FieldId::FRC_2025_REEFSCAPE_WELDED,
    FieldId::FRC_2025_REEFSCAPE_ANDY_MARK,
    FieldId::FTC_2025_2026_DECODE,
    FieldId::FRC_2026_REBUILT_WELDED,
    FieldId::FRC_2026_REBUILT_ANDY_MARK,
};

TEST_CASE("FieldsTagTest TestLoad2022RapidReact", "[field-tag][field-layout]") {
  Field field = GetField(FieldId::FRC_2022_RAPID_REACT);

  // Blue Hangar Truss - Hub
  auto expectedPose =
      wpi::math::Pose3d{127.272_in, 216.01_in, 67.932_in,
                        wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}};
  auto maybePose = field.GetTagPose(1);
  REQUIRE(maybePose);
  CHECK(expectedPose == *maybePose);

  // Blue Terminal Near Station
  expectedPose =
      wpi::math::Pose3d{4.768_in, 67.631_in, 35.063_in,
                        wpi::math::Rotation3d{0_deg, 0_deg, 46.25_deg}};
  maybePose = field.GetTagPose(5);
  REQUIRE(maybePose);
  CHECK(expectedPose == *maybePose);

  // Upper Hub Blue-Near
  expectedPose =
      wpi::math::Pose3d{332.321_in, 183.676_in, 95.186_in,
                        wpi::math::Rotation3d{0_deg, 26.75_deg, 69_deg}};
  maybePose = field.GetTagPose(53);
  REQUIRE(maybePose);
  CHECK(expectedPose == *maybePose);

  // Doesn't exist
  maybePose = field.GetTagPose(54);
  CHECK_FALSE(maybePose);
}

// Test all of the fields in the enum
TEST_CASE("FieldsTagTest CheckEntireEnum", "[field-tag][field-layout]") {
  for (auto field : ALL_FIELDS) {
    auto fieldData = GetField(field);
    if (!fieldData.HasTags()) {
      continue;
    }
    CAPTURE(fieldData.GetName());
    CHECK(fieldData.GetLength().value() > 0.0);
    CHECK(fieldData.GetWidth().value() > 0.0);
    CHECK_FALSE(fieldData.GetTags().empty());
  }
}

}  // namespace wpi::fields
