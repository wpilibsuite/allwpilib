// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/apriltag/AprilTagFields.hpp"

namespace wpi::apriltag {

std::vector<AprilTagField> GetAllFields() {
  std::vector<AprilTagField> output;

  for (int i = 0; i < static_cast<int>(AprilTagField::kNumFields); ++i) {
    output.push_back(static_cast<AprilTagField>(i));
  }

  return output;
}

TEST(AprilTagFieldsTest, TestLoad2022RapidReact) {
  AprilTagFieldLayout layout =
      AprilTagFieldLayout::LoadField(AprilTagField::k2022RapidReact);

  // Blue Hangar Truss - Hub
  auto expectedPose =
      wpi::math::Pose3d{127.272_in, 216.01_in, 67.932_in,
                        wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}};
  auto maybePose = layout.GetTagPose(1);
  EXPECT_TRUE(maybePose);
  EXPECT_EQ(expectedPose, *maybePose);

  // Blue Terminal Near Station
  expectedPose =
      wpi::math::Pose3d{4.768_in, 67.631_in, 35.063_in,
                        wpi::math::Rotation3d{0_deg, 0_deg, 46.25_deg}};
  maybePose = layout.GetTagPose(5);
  EXPECT_TRUE(maybePose);
  EXPECT_EQ(expectedPose, *maybePose);

  // Upper Hub Blue-Near
  expectedPose =
      wpi::math::Pose3d{332.321_in, 183.676_in, 95.186_in,
                        wpi::math::Rotation3d{0_deg, 26.75_deg, 69_deg}};
  maybePose = layout.GetTagPose(53);
  EXPECT_TRUE(maybePose);
  EXPECT_EQ(expectedPose, *maybePose);

  // Doesn't exist
  maybePose = layout.GetTagPose(54);
  EXPECT_FALSE(maybePose);
}

// Test all of the fields in the enum
class AllFieldsFixtureTest : public ::testing::TestWithParam<AprilTagField> {};

TEST_P(AllFieldsFixtureTest, CheckEntireEnum) {
  AprilTagField field = GetParam();
  EXPECT_NO_THROW(AprilTagFieldLayout::LoadField(field));
}

INSTANTIATE_TEST_SUITE_P(ValuesEnumTestInstTests, AllFieldsFixtureTest,
                         ::testing::ValuesIn(GetAllFields()));

}  // namespace wpi::apriltag
