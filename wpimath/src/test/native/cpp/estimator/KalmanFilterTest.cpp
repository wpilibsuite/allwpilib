// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cmath>

#include <Eigen/Core>
#include <gtest/gtest.h>

#include "units/moment_of_inertia.h"
#include "units/time.h"
#include "wpi/math/estimator/KalmanFilter.h"
#include "wpi/math/system/plant/DCMotor.h"
#include "wpi/math/system/plant/LinearSystemId.h"

TEST(KalmanFilterTest, Flywheel) {
  auto motor = wpi::math::DCMotor::NEO();
  auto flywheel =
      wpi::math::LinearSystemId::FlywheelSystem(motor, 1_kg_sq_m, 1.0);
  wpi::math::KalmanFilter<1, 1, 1> kf{flywheel, {1}, {1}, 5_ms};
}
