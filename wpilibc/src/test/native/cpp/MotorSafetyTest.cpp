// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/MotorSafety.h"  // NOLINT(build/include_order)

#include <thread>

#include <gtest/gtest.h>

#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/SimHooks.h"

using namespace frc;

class MockMotorSafety : public MotorSafety {
 public:
  MockMotorSafety() { SetSafetyEnabled(true); }

  int counter{0};

  void StopMotor() override { counter++; }

  std::string GetDescription() const override { return "test"; }
};

TEST(MotorSafetyTest, Monitor) {
  MockMotorSafety motorSafety;
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();

  motorSafety.Feed();
  EXPECT_TRUE(motorSafety.IsAlive());
  EXPECT_EQ(0, motorSafety.counter);

  for (int i = 0; i <= 6; i++) {
    frc::sim::StepTiming(0.02_s);
    frc::sim::DriverStationSim::NotifyNewData();
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  EXPECT_FALSE(motorSafety.IsAlive());
  EXPECT_EQ(1, motorSafety.counter);
}
