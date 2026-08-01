// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/FieldTag.hpp"
#include "wpi/fields/fields.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::fields;

TEST_CASE("FieldTagJsonTest DeserializeMatches", "[field-tag][json]") {
  auto field = Field{
      "2027 FRC Test Field",
      "2027",
      "Test Field",
      std::nullopt,
      54_ft,
      27_ft,
      "frc",
      std::vector{FieldTag{1, wpi::math::Pose3d{}},
                  FieldTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                                wpi::math::Rotation3d{
                                                    0_deg, 0_deg, 0_deg}}}}};

  Field deserialized;
  wpi::util::json json = field;
  CHECK_NOTHROW(deserialized = json.get<Field>());
  CHECK(field == deserialized);
}

TEST_CASE("FieldTagJsonTest MovedFromOwnedTagsAreEmpty", "[field-tag][move]") {
  auto field = Field{
      "2027 FRC Test Field",
      "2027",
      "Test Field",
      std::nullopt,
      54_ft,
      27_ft,
      "frc",
      std::vector{FieldTag{1, wpi::math::Pose3d{}},
                  FieldTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                                wpi::math::Rotation3d{
                                                    0_deg, 0_deg, 0_deg}}}}};

  {
    Field moved{std::move(field)};
    CHECK(moved.HasTags());
    CHECK(moved.GetTags().size() == 2);
  }

  CHECK_FALSE(field.HasTags());
  CHECK(field.GetTags().empty());
  CHECK_FALSE(field.GetTagPose(1));
}

TEST_CASE("FieldJsonTest DeserializeWithoutImage", "[field][json]") {
  auto json = wpi::util::json::parse(R"({
    "name": "2027 FRC Imageless Field",
    "season": "2027",
    "game": "Imageless Field",
    "field-dimensions": {
      "length": 3.0,
      "width": 2.0
    },
    "program": "frc"
  })")
                  .value();

  Field field;
  CHECK_NOTHROW(field = json.get<Field>());
  CHECK(field.GetName() == "2027 FRC Imageless Field");
  CHECK(field.GetSeason() == "2027");
  CHECK(field.GetGame() == "Imageless Field");
  CHECK_FALSE(field.HasImage());
  CHECK_FALSE(field.GetImage());
  CHECK_FALSE(field.HasTags());
  CHECK(field.GetTags().empty());
}

TEST_CASE("FieldJsonTest DeserializeImageWithoutPathFails", "[field][json]") {
  auto json = wpi::util::json::parse(R"({
    "name": "2027 FRC Pathless Image Field",
    "season": "2027",
    "game": "Pathless Image Field",
    "field-dimensions": {
      "length": 3.0,
      "width": 2.0
    },
    "field-image": {
      "top": 10,
      "left": 20,
      "bottom": 30,
      "right": 40
    },
    "program": "frc"
  })")
                  .value();

  CHECK_THROWS(json.get<Field>());
}
