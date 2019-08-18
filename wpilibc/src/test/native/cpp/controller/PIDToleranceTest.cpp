/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

using units::meter_t;

class PIDToleranceTest : public testing::Test {
 protected:
  static constexpr meter_t kSetpoint = 50_m;
  static constexpr meter_t kRange = 200_m;
  static constexpr meter_t kTolerance = 10_m;

  frc2::PIDController<meter_t>* pidController;

  void SetUp() override {
    pidController = new frc2::PIDController<meter_t>(0.5, 0.0, 0.0);
  }

  void TearDown() override { delete pidController; }
};

TEST_F(PIDToleranceTest, Initial) { EXPECT_TRUE(pidController->AtSetpoint()); }

TEST_F(PIDToleranceTest, Absolute) {
  pidController->SetTolerance(kTolerance);
  pidController->SetSetpoint(kSetpoint);

  pidController->Calculate(0.0_m);

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
