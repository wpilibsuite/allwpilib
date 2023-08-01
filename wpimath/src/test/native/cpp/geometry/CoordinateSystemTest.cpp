// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/CoordinateSystem.h"
#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Transform3d.h"
#include "gtest/gtest.h"

using namespace frc;

void CheckPose3dConvert(const Pose3d& poseFrom, const Pose3d& poseTo,
                        const CoordinateSystem& coordFrom,
                        const CoordinateSystem& coordTo) {
  // "from" to "to"
  EXPECT_EQ(
      poseTo.Translation(),
      CoordinateSystem::Convert(poseFrom.Translation(), coordFrom, coordTo));
  EXPECT_EQ(poseTo.Rotation(),
            CoordinateSystem::Convert(poseFrom.Rotation(), coordFrom, coordTo));
  EXPECT_EQ(poseTo, CoordinateSystem::Convert(poseFrom, coordFrom, coordTo));

  // "to" to "from"
  EXPECT_EQ(
      poseFrom.Translation(),
      CoordinateSystem::Convert(poseTo.Translation(), coordTo, coordFrom));
  EXPECT_EQ(poseFrom.Rotation(),
            CoordinateSystem::Convert(poseTo.Rotation(), coordTo, coordFrom));
  EXPECT_EQ(poseFrom, CoordinateSystem::Convert(poseTo, coordTo, coordFrom));
}

void CheckTransform3dConvert(const Transform3d& transformFrom,
                             const Transform3d& transformTo,
                             const CoordinateSystem& coordFrom,
                             const CoordinateSystem& coordTo) {
  // "from" to "to"
  EXPECT_EQ(transformTo.Translation(),
            CoordinateSystem::Convert(transformFrom.Translation(), coordFrom,
                                      coordTo));
  EXPECT_EQ(
      transformTo.Rotation(),
      CoordinateSystem::Convert(transformFrom.Rotation(), coordFrom, coordTo));
  EXPECT_EQ(transformTo,
            CoordinateSystem::Convert(transformFrom, coordFrom, coordTo));

  // "to" to "from"
  EXPECT_EQ(
      transformFrom.Translation(),
      CoordinateSystem::Convert(transformTo.Translation(), coordTo, coordFrom));
  EXPECT_EQ(
      transformFrom.Rotation(),
      CoordinateSystem::Convert(transformTo.Rotation(), coordTo, coordFrom));
  EXPECT_EQ(transformFrom,
            CoordinateSystem::Convert(transformTo, coordTo, coordFrom));
}

TEST(CoordinateSystemTest, Pose3dEDNtoNWU) {
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

TEST(CoordinateSystemTest, Pose3dEDNtoNED) {
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

TEST(CoordinateSystemTest, Transform3dEDNtoNWU) {
  // No rotation from EDN to NWU
  CheckTransform3dConvert(
      Transform3d{Translation3d{1_m, 2_m, 3_m}, Rotation3d{}},
      Transform3d{Translation3d{3_m, -1_m, -2_m},
                  Rotation3d{-90_deg, 0_deg, -90_deg}},
      CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° roll from EDN to NWU
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{45_deg, 0_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, -1_m, -2_m},
                                      Rotation3d{-45_deg, 0_deg, -90_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° pitch from EDN to NWU
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 45_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, -1_m, -2_m},
                                      Rotation3d{-90_deg, 0_deg, -135_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NWU());

  // 45° yaw from EDN to NWU
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 0_deg, 45_deg}},
                          Transform3d{Translation3d{3_m, -1_m, -2_m},
                                      Rotation3d{-90_deg, 45_deg, -90_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NWU());
}

TEST(CoordinateSystemTest, Transform3dEDNtoNED) {
  // No rotation from EDN to NED
  CheckTransform3dConvert(
      Transform3d{Translation3d{1_m, 2_m, 3_m}, Rotation3d{}},
      Transform3d{Translation3d{3_m, 1_m, 2_m},
                  Rotation3d{90_deg, 0_deg, 90_deg}},
      CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° roll from EDN to NED
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{45_deg, 0_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, 1_m, 2_m},
                                      Rotation3d{135_deg, 0_deg, 90_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° pitch from EDN to NED
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 45_deg, 0_deg}},
                          Transform3d{Translation3d{3_m, 1_m, 2_m},
                                      Rotation3d{90_deg, 0_deg, 135_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NED());

  // 45° yaw from EDN to NED
  CheckTransform3dConvert(Transform3d{Translation3d{1_m, 2_m, 3_m},
                                      Rotation3d{0_deg, 0_deg, 45_deg}},
                          Transform3d{Translation3d{3_m, 1_m, 2_m},
                                      Rotation3d{90_deg, -45_deg, 90_deg}},
                          CoordinateSystem::EDN(), CoordinateSystem::NED());
}
