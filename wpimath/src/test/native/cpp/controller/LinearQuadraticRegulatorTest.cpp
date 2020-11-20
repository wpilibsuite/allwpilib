/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <cmath>

#include "Eigen/Core"
#include "frc/controller/LinearQuadraticRegulator.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/time.h"

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

    return frc::LinearSystemId::ElevatorSystem(motors, m, r, G);
  }();
  LinearQuadraticRegulator<2, 1> controller{
      plant, {0.02, 0.4}, {12.0}, 0.00505_s};

  EXPECT_NEAR(522.15314269, controller.K(0, 0), 1e-6);
  EXPECT_NEAR(38.20138596, controller.K(0, 1), 1e-6);
}

TEST(LinearQuadraticRegulatorTest, ArmGains) {
  LinearSystem<2, 1, 1> plant = [] {
    auto motors = DCMotor::Vex775Pro(2);

    // Carriage mass
    constexpr auto m = 4_kg;

    // Radius of pulley
    constexpr auto r = 0.4_m;

    // Gear ratio
    constexpr double G = 100.0;

    return frc::LinearSystemId::SingleJointedArmSystem(
        motors, 1.0 / 3.0 * m * r * r, G);
  }();

  LinearQuadraticRegulator<2, 1> controller{
      plant, {0.01745, 0.08726}, {12.0}, 0.00505_s};

  EXPECT_NEAR(19.16, controller.K(0, 0), 1e-1);
  EXPECT_NEAR(3.32, controller.K(0, 1), 1e-1);
}

TEST(LinearQuadraticRegulatorTest, FourMotorElevator) {
  LinearSystem<2, 1, 1> plant = [] {
    auto motors = DCMotor::Vex775Pro(4);

    // Carriage mass
    constexpr auto m = 8_kg;

    // Radius of pulley
    constexpr auto r = 0.75_in;

    // Gear ratio
    constexpr double G = 14.67;

    return frc::LinearSystemId::ElevatorSystem(motors, m, r, G);
  }();
  LinearQuadraticRegulator<2, 1> controller{plant, {0.1, 0.2}, {12.0}, 0.020_s};

  EXPECT_NEAR(10.38, controller.K(0, 0), 1e-1);
  EXPECT_NEAR(0.69, controller.K(0, 1), 1e-1);
}

TEST(LinearQuadraticRegulatorTest, LatencyCompensate) {
  LinearSystem<2, 1, 1> plant = [] {
    auto motors = DCMotor::Vex775Pro(4);

    // Carriage mass
    constexpr auto m = 8_kg;

    // Radius of pulley
    constexpr auto r = 0.75_in;

    // Gear ratio
    constexpr double G = 14.67;

    return frc::LinearSystemId::ElevatorSystem(motors, m, r, G);
  }();
  LinearQuadraticRegulator<2, 1> controller{plant, {0.1, 0.2}, {12.0}, 0.02_s};

  controller.LatencyCompensate(plant, 0.02_s, 0.01_s);

  EXPECT_NEAR(8.97115941, controller.K(0, 0), 1e-3);
  EXPECT_NEAR(0.07904881, controller.K(0, 1), 1e-3);
}

}  // namespace frc
