// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/Ultrasonic.h"
#include "frc/simulation/UltrasonicSim.h"

using namespace frc;

TEST(UltrasonicTest, SimDevices) {
  Ultrasonic ultrasonic{0, 1};
  sim::UltrasonicSim sim{ultrasonic};

  EXPECT_EQ(0, ultrasonic.GetRange().value());
  EXPECT_TRUE(ultrasonic.IsRangeValid());

  sim.SetRange(units::meter_t{35.04});
  EXPECT_EQ(35.04, ultrasonic.GetRange().value());

  sim.SetRangeValid(false);
  EXPECT_FALSE(ultrasonic.IsRangeValid());
  EXPECT_EQ(0, ultrasonic.GetRange().value());
}

TEST(UltrasonicTest, AutomaticModeToggle) {
  frc::Ultrasonic ultrasonic{0, 1};
  EXPECT_NO_THROW({
    frc::Ultrasonic::SetAutomaticMode(true);
    frc::Ultrasonic::SetAutomaticMode(false);
    frc::Ultrasonic::SetAutomaticMode(true);
  });
}

TEST(UltrasonicTest, AutomaticModeOnWithZeroInstances) {
  EXPECT_NO_THROW({ frc::Ultrasonic::SetAutomaticMode(true); });
}

TEST(UltrasonicTest, AutomaticModeOffWithZeroInstances) {
  EXPECT_NO_THROW({ frc::Ultrasonic::SetAutomaticMode(false); });
}
