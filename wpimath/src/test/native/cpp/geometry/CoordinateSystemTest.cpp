// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/CoordinateSystem.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Transform3d.hpp"

using namespace wpi::math;

void CheckPose3dConvert(const Pose3d& poseFrom, const Pose3d& poseTo,
                        const CoordinateSystem& coordFrom,
                        const CoordinateSystem& coordTo) {
  // "from" to "to"
  CHECK(poseTo.Translation() ==
        CoordinateSystem::Convert(poseFrom.Translation(), coordFrom, coordTo));
  CHECK(poseTo.Rotation() ==
        CoordinateSystem::Convert(poseFrom.Rotation(), coordFrom, coordTo));
  CHECK(poseTo == CoordinateSystem::Convert(poseFrom, coordFrom, coordTo));

  // "to" to "from"
  CHECK(poseFrom.Translation() ==
        CoordinateSystem::Convert(poseTo.Translation(), coordTo, coordFrom));
  CHECK(poseFrom.Rotation() ==
        CoordinateSystem::Convert(poseTo.Rotation(), coordTo, coordFrom));
  CHECK(poseFrom == CoordinateSystem::Convert(poseTo, coordTo, coordFrom));
}

void CheckTransform3dConvert(const Transform3d& transformFrom,
                             const Transform3d& transformTo,
                             const CoordinateSystem& coordFrom,
                             const CoordinateSystem& coordTo) {
  // "from" to "to"
  CHECK(transformTo.Translation() ==
        CoordinateSystem::Convert(transformFrom.Translation(), coordFrom,
                                  coordTo));
  CHECK(transformTo ==
        CoordinateSystem::Convert(transformFrom, coordFrom, coordTo));

  // "to" to "from"
  CHECK(
      transformFrom.Translation() ==
      CoordinateSystem::Convert(transformTo.Translation(), coordTo, coordFrom));
  CHECK(transformFrom ==
        CoordinateSystem::Convert(transformTo, coordTo, coordFrom));
}

TEST_CASE("CoordinateSystemTest Pose3dEDNtoNWU", "[wpimath]") {
  // No rotation from EDN to NWU
  CheckPose3dConvert(
      Pose3d{1_m, 2_m, 3_m, Rotation3d{}},
      Pose3d{3_m, -1_m, -2_m, Rotation3d{-90_deg, 0_deg, -90_deg}},
      CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° roll from EDN to NWU
  CheckPose3dConvert(
      Pose3d{1_m, 2_m, 3_m, Rotation3d{45_deg, 0_deg, 0_deg}},
      Pose3d{3_m, -1_m, -2_m, Rotation3d{-45_deg, 0_deg, -90_deg}},
      CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° pitch from EDN to NWU
  CheckPose3dConvert(
      Pose3d{1_m, 2_m, 3_m, Rotation3d{0_deg, 45_deg, 0_deg}},
      Pose3d{3_m, -1_m, -2_m, Rotation3d{-90_deg, 0_deg, -135_deg}},
      CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° yaw from EDN to NWU
  CheckPose3dConvert(
      Pose3d{1_m, 2_m, 3_m, Rotation3d{0_deg, 0_deg, 45_deg}},
      Pose3d{3_m, -1_m, -2_m, Rotation3d{-90_deg, 45_deg, -90_deg}},
      CoordinateSystem::EDN(), CoordinateSystem::NWU());
}

TEST_CASE("CoordinateSystemTest Pose3dEDNtoNED", "[wpimath]") {
  // No rotation from EDN to NED
  CheckPose3dConvert(Pose3d{1_m, 2_m, 3_m, Rotation3d{}},
                     Pose3d{3_m, 1_m, 2_m, Rotation3d{90_deg, 0_deg, 90_deg}},
                     CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° roll from EDN to NED
  CheckPose3dConvert(Pose3d{1_m, 2_m, 3_m, Rotation3d{45_deg, 0_deg, 0_deg}},
                     Pose3d{3_m, 1_m, 2_m, Rotation3d{135_deg, 0_deg, 90_deg}},
                     CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° pitch from EDN to NED
  CheckPose3dConvert(Pose3d{1_m, 2_m, 3_m, Rotation3d{0_deg, 45_deg, 0_deg}},
                     Pose3d{3_m, 1_m, 2_m, Rotation3d{90_deg, 0_deg, 135_deg}},
                     CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° yaw from EDN to NED
  CheckPose3dConvert(Pose3d{1_m, 2_m, 3_m, Rotation3d{0_deg, 0_deg, 45_deg}},
                     Pose3d{3_m, 1_m, 2_m, Rotation3d{90_deg, -45_deg, 90_deg}},
                     CoordinateSystem::EDN(), CoordinateSystem::NED());
}

TEST_CASE("CoordinateSystemTest Transform3dEDNtoNWU", "[wpimath]") {
  // No rotation from EDN to NWU
  CheckTransform3dConvert(
      Transform3d{Translation3d{1_m, 2_m, 3_m}, Rotation3d{}},
      Transform3d{Translation3d{3_m, -1_m, -2_m}, Rotation3d{}},
      CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° roll from EDN to NWU
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{45_deg, 0_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, -1_m, -2_m},
                                      Rotation3d{0_deg, -45_deg, 0_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° pitch from EDN to NWU
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 45_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, -1_m, -2_m},
                                      Rotation3d{0_deg, 0_deg, -45_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° yaw from EDN to NWU
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 0_deg, 45_deg}},
                          Transform3d{Translation3d{3_m, -1_m, -2_m},
                                      Rotation3d{45_deg, 0_deg, 0_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NWU());
}

TEST_CASE("CoordinateSystemTest Transform3dEDNtoNED", "[wpimath]") {
  // No rotation from EDN to NED
  CheckTransform3dConvert(
      Transform3d{Translation3d{1_m, 2_m, 3_m}, Rotation3d{}},
      Transform3d{Translation3d{3_m, 1_m, 2_m}, Rotation3d{}},
      CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° roll from EDN to NED
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{45_deg, 0_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, 1_m, 2_m},
                                      Rotation3d{0_deg, 45_deg, 0_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° pitch from EDN to NED
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 45_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, 1_m, 2_m},
                                      Rotation3d{0_deg, 0_deg, 45_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° yaw from EDN to NED
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 0_deg, 45_deg}},
                          Transform3d{Translation3d{3_m, 1_m, 2_m},
                                      Rotation3d{45_deg, 0_deg, 0_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NED());
}

TEST_CASE("CoordinateSystemTest LeftHandedSystemThrowsException", "[wpimath]") {
  CHECK_THROWS_AS(CoordinateSystem(CoordinateAxis::N(), CoordinateAxis::E(),
                                   CoordinateAxis::U()),
                  std::domain_error);
  CHECK_THROWS_AS(CoordinateSystem(CoordinateAxis::E(), CoordinateAxis::U(),
                                   CoordinateAxis::N()),
                  std::domain_error);
  CHECK_THROWS_AS(CoordinateSystem(CoordinateAxis::N(), CoordinateAxis::W(),
                                   CoordinateAxis::D()),
                  std::domain_error);
}
