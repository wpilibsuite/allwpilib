// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/PWMMotorControllerSim.hpp"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include "wpi/hal/HAL.h"

#include "wpi/hardware/motor/Spark.hpp"

namespace frc::sim {
TEST(PWMMotorControllerSimTest, TestMotor) {
  frc::Spark spark{0};
  frc::sim::PWMMotorControllerSim sim{spark};

  spark.Set(0);
  EXPECT_EQ(0, sim.GetSpeed());

  spark.Set(0.354);
  EXPECT_EQ(0.354, sim.GetSpeed());

  spark.Set(-0.785);
  EXPECT_EQ(-0.785, sim.GetSpeed());
}
}  // namespace frc::sim
