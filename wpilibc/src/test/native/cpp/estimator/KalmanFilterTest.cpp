/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <array>
#include <cmath>

#include <Eigen/Core>

#include "frc/estimator/KalmanFilter.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/FlywheelSystem.h"

TEST(KalmanFilterTest, Flywheel) {
  auto motor = frc::DCMotor::NEO();
  auto flywheel = frc::FlywheelSystem(motor, 1_kg_sq_m, 1.0);
  frc::KalmanFilter<1, 1, 1> kf{flywheel, 5_ms, {1}, {1}};
}
