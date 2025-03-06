// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PWMMotorControllerSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/motorcontrol/Spark.h"

namespace frc::sim {
TEST(PWMMotorControllerSimTest, TestMotor) {
  frc::Spark spark{0};
  frc::PWMMotorControllerSim sim{spark};

  spark.Set(0);
  EXPECT_EQ(0, sim.GetSpeed());

  spark.Set(0.354);
  EXPECT_EQ(0.354, sim.GetSpeed());

  spark.Set(-0.785);
  EXPECT_EQ(-0.785, sim.GetSpeed());
}
}
