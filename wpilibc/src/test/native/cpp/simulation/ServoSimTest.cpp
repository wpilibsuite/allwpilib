// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/ServoSim.hpp"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <wpi/hal/HAL.hpp>

#include "wpi/hardware/servo/Servo.hpp"

namespace wpi::sim {
TEST(ServoSimTest, TestServo) {
  wpi::Servo servo{0};
  wpi::sim::ServoSim sim{servo};

  servo.Set(0);
  EXPECT_EQ(0, sim.GetPosition());

  servo.Set(0.354);
  EXPECT_EQ(0.354, sim.GetPosition());

  servo.SetAngle(10);
  EXPECT_EQ(10, sim.GetAngle());

  servo.SetAngle(90);
  EXPECT_EQ(90, sim.GetAngle());

  servo.SetAngle(170);
  EXPECT_EQ(170, sim.GetAngle());
}
}  // namespace wpi::sim
