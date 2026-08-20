// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/Field2d.hpp"

#include <memory>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/units/angle.hpp"

struct Field2dTest {
  Field2dTest() {
    wpi::tunables::TunableRegistry::Reset();
    wpi::tunables::TunableRegistry::RegisterBackend("", mock);
  }

  ~Field2dTest() { wpi::tunables::TunableRegistry::Reset(); }

  std::shared_ptr<wpi::tunables::MockTunableBackend> mock =
      std::make_shared<wpi::tunables::MockTunableBackend>();
};

static void CheckPose(const wpi::math::Pose2d& actual,
                      const wpi::math::Pose2d& expected) {
  CHECK(actual.X() == expected.X());
  CHECK(actual.Y() == expected.Y());
  CHECK(actual.Rotation().Degrees() == expected.Rotation().Degrees());
}

TEST_CASE_METHOD(Field2dTest,
                 "Field2dTest DashboardEditsRobotPoseThroughTunable",
                 "[wpilibc][smartdashboard]") {
  wpi::Field2d field;
  wpi::tunables::Publish("field", field);

  wpi::math::Pose2d pose{1_m, 2_m, 30_deg};
  mock->SetStructVector<wpi::math::Pose2d>("/field/Robot", std::vector{pose});
  wpi::tunables::TunableRegistry::Update();

  CheckPose(field.GetRobotPose(), pose);
}

TEST_CASE_METHOD(Field2dTest,
                 "Field2dTest DashboardEditsObjectPosesThroughTunable",
                 "[wpilibc][smartdashboard]") {
  wpi::Field2d field;
  wpi::FieldObject2d* existing = field.GetObject("target");
  wpi::tunables::Publish("field", field);

  wpi::math::Pose2d first{1_m, 2_m, 30_deg};
  wpi::math::Pose2d second{3_m, 4_m, 60_deg};

  mock->SetStructVector<wpi::math::Pose2d>("/field/target",
                                           std::vector{first, second});
  wpi::tunables::TunableRegistry::Update();

  auto existingPoses = existing->GetPoses();
  REQUIRE(existingPoses.size() == 2u);
  CheckPose(existingPoses[0], first);
  CheckPose(existingPoses[1], second);
}

TEST_CASE_METHOD(Field2dTest,
                 "Field2dTest DashboardEditsObjectCreatedAfterTunablePublish",
                 "[wpilibc][smartdashboard]") {
  wpi::Field2d field;
  wpi::tunables::Publish("field", field);
  wpi::FieldObject2d* late = field.GetObject("target");

  wpi::math::Pose2d pose{1_m, 2_m, 30_deg};
  mock->SetStructVector<wpi::math::Pose2d>("/field/target", std::vector{pose});
  wpi::tunables::TunableRegistry::Update();

  auto latePoses = late->GetPoses();
  REQUIRE(latePoses.size() == 1u);
  CheckPose(latePoses[0], pose);
}

TEST_CASE_METHOD(Field2dTest,
                 "Field2dTest MoveAssignPublishedFromUnpublishedRepublishes"
                 "ObjectTunables",
                 "[wpilibc][smartdashboard]") {
  wpi::Field2d field;
  field.GetObject("stale");
  wpi::tunables::Publish("field", field);

  {
    wpi::Field2d replacement;
    wpi::math::Pose2d robotPose{1_m, 2_m, 30_deg};
    wpi::math::Pose2d targetPose{3_m, 4_m, 60_deg};
    replacement.SetRobotPose(robotPose);
    replacement.GetObject("target")->SetPose(targetPose);

    field = std::move(replacement);
  }

  CHECK_FALSE(mock->GetUid("/field/stale").has_value());

  auto robotPoses = mock->GetStructVector<wpi::math::Pose2d>("/field/Robot");
  REQUIRE(robotPoses.size() == 1u);
  CheckPose(robotPoses[0], {1_m, 2_m, 30_deg});

  auto targetPoses = mock->GetStructVector<wpi::math::Pose2d>("/field/target");
  REQUIRE(targetPoses.size() == 1u);
  CheckPose(targetPoses[0], {3_m, 4_m, 60_deg});

  wpi::math::Pose2d editedPose{5_m, 6_m, 90_deg};
  mock->SetStructVector<wpi::math::Pose2d>("/field/target",
                                           std::vector{editedPose});
  wpi::tunables::TunableRegistry::Update();

  auto movedTargetPoses = field.GetObject("target")->GetPoses();
  REQUIRE(movedTargetPoses.size() == 1u);
  CheckPose(movedTargetPoses[0], editedPose);
}
