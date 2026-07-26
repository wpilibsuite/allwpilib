// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/drivers/motor/Spark.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"

TEST(PWMMotorControllerTest, ReportsThrottleToSimulation) {
  wpi::Spark motor{0};
  wpi::sim::PWMMotorControllerSim sim{motor};

  motor.SetThrottle(0.354);

  EXPECT_EQ(0.354, sim.GetThrottle());
}
