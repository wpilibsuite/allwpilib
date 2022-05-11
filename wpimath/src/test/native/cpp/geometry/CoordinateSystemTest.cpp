// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/CoordinateSystem.h"
#include "frc/geometry/Pose3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(CoordinateSystemTest, EDNtoNWU) {
  Pose3d edn1{1_m, 2_m, 3_m, Rotation3d{}};
  Pose3d nwu1{3_m, -1_m, -2_m, Rotation3d{-90_deg, 0_deg, -90_deg}};

  EXPECT_EQ(nwu1, CoordinateSystem::Convert(edn1, CoordinateSystem::EDN(),
                                            CoordinateSystem::NWU()));
  EXPECT_EQ(edn1, CoordinateSystem::Convert(nwu1, CoordinateSystem::NWU(),
                                            CoordinateSystem::EDN()));

  Pose3d edn2{1_m, 2_m, 3_m, Rotation3d{45_deg, 0_deg, 0_deg}};
  Pose3d nwu2{3_m, -1_m, -2_m, Rotation3d{-45_deg, 0_deg, -90_deg}};

  EXPECT_EQ(nwu2, CoordinateSystem::Convert(edn2, CoordinateSystem::EDN(),
                                            CoordinateSystem::NWU()));
  EXPECT_EQ(edn2, CoordinateSystem::Convert(nwu2, CoordinateSystem::NWU(),
                                            CoordinateSystem::EDN()));

  Pose3d edn3{1_m, 2_m, 3_m, Rotation3d{0_deg, 45_deg, 0_deg}};
  Pose3d nwu3{3_m, -1_m, -2_m, Rotation3d{-90_deg, 0_deg, -135_deg}};

  EXPECT_EQ(nwu3, CoordinateSystem::Convert(edn3, CoordinateSystem::EDN(),
                                            CoordinateSystem::NWU()));
  EXPECT_EQ(edn3, CoordinateSystem::Convert(nwu3, CoordinateSystem::NWU(),
                                            CoordinateSystem::EDN()));

  Pose3d edn4{1_m, 2_m, 3_m, Rotation3d{0_deg, 0_deg, 45_deg}};
  Pose3d nwu4{3_m, -1_m, -2_m, Rotation3d{-90_deg, 45_deg, -90_deg}};

  EXPECT_EQ(nwu4, CoordinateSystem::Convert(edn4, CoordinateSystem::EDN(),
                                            CoordinateSystem::NWU()));
  EXPECT_EQ(edn4, CoordinateSystem::Convert(nwu4, CoordinateSystem::NWU(),
                                            CoordinateSystem::EDN()));
}

TEST(CoordinateSystemTest, EDNtoNED) {
  Pose3d edn1{1_m, 2_m, 3_m, Rotation3d{}};
  Pose3d ned1{3_m, 1_m, 2_m, Rotation3d{90_deg, 0_deg, 90_deg}};

  EXPECT_EQ(ned1, CoordinateSystem::Convert(edn1, CoordinateSystem::EDN(),
                                            CoordinateSystem::NED()));
  EXPECT_EQ(edn1, CoordinateSystem::Convert(ned1, CoordinateSystem::NED(),
                                            CoordinateSystem::EDN()));

  Pose3d edn2{1_m, 2_m, 3_m, Rotation3d{45_deg, 0_deg, 0_deg}};
  Pose3d ned2{3_m, 1_m, 2_m, Rotation3d{135_deg, 0_deg, 90_deg}};

  EXPECT_EQ(ned2, CoordinateSystem::Convert(edn2, CoordinateSystem::EDN(),
                                            CoordinateSystem::NED()));
  EXPECT_EQ(edn2, CoordinateSystem::Convert(ned2, CoordinateSystem::NED(),
                                            CoordinateSystem::EDN()));

  Pose3d edn3{1_m, 2_m, 3_m, Rotation3d{0_deg, 45_deg, 0_deg}};
  Pose3d ned3{3_m, 1_m, 2_m, Rotation3d{90_deg, 0_deg, 135_deg}};

  EXPECT_EQ(ned3, CoordinateSystem::Convert(edn3, CoordinateSystem::EDN(),
                                            CoordinateSystem::NED()));
  EXPECT_EQ(edn3, CoordinateSystem::Convert(ned3, CoordinateSystem::NED(),
                                            CoordinateSystem::EDN()));

  Pose3d edn4{1_m, 2_m, 3_m, Rotation3d{0_deg, 0_deg, 45_deg}};
  Pose3d ned4{3_m, 1_m, 2_m, Rotation3d{90_deg, -45_deg, 90_deg}};

  EXPECT_EQ(ned4, CoordinateSystem::Convert(edn4, CoordinateSystem::EDN(),
                                            CoordinateSystem::NED()));
  EXPECT_EQ(edn4, CoordinateSystem::Convert(ned4, CoordinateSystem::NED(),
                                            CoordinateSystem::EDN()));
}
