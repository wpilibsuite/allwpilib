// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cmath>

#include <Eigen/Core>
#include <gtest/gtest.h>

#include "units/moment_of_inertia.h"
#include "units/time.h"
#include "wpimath/estimator/KalmanFilter.h"
#include "wpimath/system/plant/DCMotor.h"
#include "wpimath/system/plant/LinearSystemId.h"

TEST(KalmanFilterTest, Flywheel) {
  auto motor = wpimath::DCMotor::NEO();
  auto flywheel =
      wpimath::LinearSystemId::FlywheelSystem(motor, 1_kg_sq_m, 1.0);
  wpimath::KalmanFilter<1, 1, 1> kf{flywheel, {1}, {1}, 5_ms};
}
