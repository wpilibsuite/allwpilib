// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/simulation/PWMMotorControllerSim.hpp"
// clang-format on

#include <gtest/gtest.h>

#include "wpi/hardware/motor/Spark.hpp"

namespace wpi::sim {
TEST(PWMMotorControllerSimTest, TestMotor) {
  wpi::Spark spark{0};
  wpi::sim::PWMMotorControllerSim sim{spark};

  spark.Set(0);
  EXPECT_EQ(0, sim.GetSpeed());

  spark.Set(0.354);
  EXPECT_EQ(0.354, sim.GetSpeed());

  spark.Set(-0.785);
  EXPECT_EQ(-0.785, sim.GetSpeed());
}
}  // namespace wpi::sim
