/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

static constexpr double kSetpoint = 50.0;
static constexpr double kRange = 200;
static constexpr double kTolerance = 10.0;

TEST(PIDToleranceTest, InitialTolerance) {
  frc2::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  EXPECT_TRUE(controller.AtSetpoint());
}

TEST(PIDToleranceTest, AbsoluteTolerance) {
  frc2::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  EXPECT_TRUE(controller.AtSetpoint())
      << "Error was not in tolerance when it should have been. Error was "
      << controller.GetPositionError();

  controller.SetTolerance(kTolerance);
  controller.SetSetpoint(kSetpoint);

  EXPECT_FALSE(controller.AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << controller.GetPositionError();

  controller.Calculate(0.0);

  EXPECT_FALSE(controller.AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << controller.GetPositionError();

  controller.Calculate(kSetpoint + kTolerance / 2);

  EXPECT_TRUE(controller.AtSetpoint())
      << "Error was not in tolerance when it should have been. Error was "
      << controller.GetPositionError();

  controller.Calculate(kSetpoint + 10 * kTolerance);

  EXPECT_FALSE(controller.AtSetpoint())
      << "Error was in tolerance when it should not have been. Error was "
      << controller.GetPositionError();
}
