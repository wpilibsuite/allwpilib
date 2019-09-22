/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <cmath>

#include <Eigen/Core>

#include "frc/controller/LinearQuadraticRegulator.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/ElevatorSystem.h"

namespace frc {

TEST(LinearQuadraticRegulatorTest, ElevatorGains) {
  LinearSystem<2, 1, 1> plant = [] {
    auto motors = DCMotor::Vex775Pro(2);

    // Carriage mass
    constexpr auto m = 5_kg;

    // Radius of pulley
    constexpr auto r = 0.0181864_m;

    // Gear ratio
    constexpr double G = 40.0 / 40.0;

    return ElevatorSystem(motors, m, r, G);
  }();
  LinearQuadraticRegulator<2, 1> controller{
      plant, {0.02, 0.4}, {12.0}, 0.00505_s};

  EXPECT_NEAR(522.15314269, controller.K(0, 0), 1e-6);
  EXPECT_NEAR(38.20138596, controller.K(0, 1), 1e-6);
}

}  // namespace frc
