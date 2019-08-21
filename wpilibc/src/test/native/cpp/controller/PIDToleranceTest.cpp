/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

class PIDToleranceTest : public testing::Test {
 protected:
  static constexpr double kSetpoint = 50.0;
  static constexpr double kRange = 200;
  static constexpr double kTolerance = 10.0;

  frc2::PIDController* pidController;

  void SetUp() override {
    pidController = new frc2::PIDController(0.5, 0.0, 0.0);
    pidController->SetInputRange(-kRange / 2, kRange / 2);
  }

  void TearDown() override { delete pidController; }
};

TEST_F(PIDToleranceTest, Initial) { EXPECT_TRUE(pidController->AtSetpoint()); }

TEST_F(PIDToleranceTest, Absolute) {
  pidController->SetAbsoluteTolerance(kTolerance);
  pidController->SetSetpoint(kSetpoint);

  pidController->Calculate(0.0);

  EXPECT_FALSE(pidController->AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetPositionError();

  pidController->Calculate(kSetpoint + kTolerance / 2);

  EXPECT_TRUE(pidController->AtSetpoint())
      << "Error was not in tolerance when it should have been. Error was "
      << pidController->GetPositionError();

  pidController->Calculate(kSetpoint + 10 * kTolerance);

  EXPECT_FALSE(pidController->AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetPositionError();
}

TEST_F(PIDToleranceTest, Percent) {
  pidController->SetPercentTolerance(kTolerance);
  pidController->SetSetpoint(kSetpoint);

  pidController->Calculate(0.0);

  EXPECT_FALSE(pidController->AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetPositionError();

  // Half of percent tolerance away from setpoint
  pidController->Calculate(kSetpoint + (kTolerance / 2) / 100 * kRange);

  EXPECT_TRUE(pidController->AtSetpoint())
      << "Error was not in tolerance when it should have been. Error was "
      << pidController->GetPositionError();

  // Double percent tolerance away from setpoint
  pidController->Calculate(kSetpoint + (kTolerance * 2) / 100 * kRange);

  EXPECT_FALSE(pidController->AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << pidController->GetPositionError();
}
