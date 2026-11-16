// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/fields/FieldTag.hpp"
#include "wpi/fields/fields.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::fields;

TEST_CASE("FieldTagPoseSetOriginTest TransformationMatches",
          "[field-tag][field-layout][pose]") {
  auto field = Field{
      "2027 FRC Test Field",
      "2027",
      "Test Field",
      std::nullopt,
      54_ft,
      27_ft,
      "frc",
      std::vector<FieldTag>{
          FieldTag{
              1, wpi::math::Pose3d{0_ft, 0_ft, 0_ft,
                                   wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}}},
          FieldTag{2, wpi::math::Pose3d{
                          4_ft, 4_ft, 4_ft,
                          wpi::math::Rotation3d{0_deg, 0_deg, 180_deg}}}}};

  CHECK(field.GetOrigin() == wpi::math::Pose3d{});
  CHECK(*field.GetTagPose(1) == wpi::math::Pose3d{});

  field.SetOrigin(Field::OriginPosition::RED_ALLIANCE_WALL_RIGHT_SIDE);
  CHECK(*field.GetTagPose(1) ==
        wpi::math::Pose3d{27_ft, 13.5_ft, 0_ft, wpi::math::Rotation3d{}});
  CHECK(*field.GetTagPose(2) ==
        wpi::math::Pose3d{31_ft, 17.5_ft, 4_ft,
                          wpi::math::Rotation3d{0_deg, 0_deg, 180_deg}});

  field.SetOrigin(Field::OriginPosition::BLUE_ALLIANCE_WALL_RIGHT_SIDE);
  CHECK(*field.GetTagPose(1) ==
        wpi::math::Pose3d{27_ft, 13.5_ft, 0_ft,
                          wpi::math::Rotation3d{0_deg, 0_deg, 180_deg}});
  CHECK(*field.GetTagPose(2) ==
        wpi::math::Pose3d{23_ft, 9.5_ft, 4_ft, wpi::math::Rotation3d{}});

  auto origin = wpi::math::Pose3d{
      -2_m, 1_m, 0.5_m, wpi::math::Rotation3d{10_deg, 20_deg, 30_deg}};
  field.SetOrigin(origin);
  CHECK(*field.GetTagPose(2) == field.GetTags()[1].pose.RelativeTo(origin));

  field.SetOrigin(Field::OriginPosition::FIELD_CENTER);
  CHECK(field.GetOrigin() == wpi::math::Pose3d{});
  CHECK(*field.GetTagPose(2) == field.GetTags()[1].pose);
}
