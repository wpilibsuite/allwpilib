// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/hardware/motor/PWMVictorSPX.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/simulation/BatterySim.hpp"
#include "wpi/simulation/DCMotorSim.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/simulation/RoboRioSim.hpp"
#include "wpi/system/RobotController.hpp"

TEST(DCMotorSimTest, VoltageSteadyState) {
  wpi::math::DCMotor gearbox = wpi::math::DCMotor::NEO(1);
  auto plant = wpi::math::LinearSystemId::DCMotorSystem(
      wpi::math::DCMotor::NEO(1), wpi::units::kilogram_square_meter_t{0.0005},
      1.0);
  wpi::sim::DCMotorSim sim{plant, gearbox};

  wpi::Encoder encoder{0, 1};
  wpi::sim::EncoderSim encoderSim{encoder};
  wpi::PWMVictorSPX motor{0};

  wpi::sim::RoboRioSim::ResetData();
  encoderSim.ResetData();

  // Spin-up
  for (int i = 0; i < 100; i++) {
    // RobotPeriodic runs first
    motor.SetVoltage(12_V);

    // Then, SimulationPeriodic runs
    wpi::sim::RoboRioSim::SetVInVoltage(
        wpi::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInputVoltage(motor.Get() *
                        wpi::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR((gearbox.Kv * 12_V).value(), encoder.GetRate(), 0.1);

  // Decay
  for (int i = 0; i < 100; i++) {
    // RobotPeriodic runs first
    motor.SetVoltage(0_V);

    // Then, SimulationPeriodic runs
    wpi::sim::RoboRioSim::SetVInVoltage(
        wpi::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInputVoltage(motor.Get() *
                        wpi::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR(0, encoder.GetRate(), 0.1);
}

TEST(DCMotorSimTest, PositionFeedbackControl) {
  wpi::math::DCMotor gearbox = wpi::math::DCMotor::NEO(1);
  auto plant = wpi::math::LinearSystemId::DCMotorSystem(
      wpi::math::DCMotor::NEO(1), wpi::units::kilogram_square_meter_t{0.0005},
      1.0);
  wpi::sim::DCMotorSim sim{plant, gearbox};

  wpi::math::PIDController controller{0.04, 0.0, 0.001};

  wpi::Encoder encoder{0, 1};
  wpi::sim::EncoderSim encoderSim{encoder};
  wpi::PWMVictorSPX motor{0};

  wpi::sim::RoboRioSim::ResetData();
  encoderSim.ResetData();

  for (int i = 0; i < 140; i++) {
    // RobotPeriodic runs first
    motor.Set(controller.Calculate(encoder.GetDistance(), 750));

    // Then, SimulationPeriodic runs
    wpi::sim::RoboRioSim::SetVInVoltage(
        wpi::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInputVoltage(motor.Get() *
                        wpi::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetDistance(sim.GetAngularPosition().value());
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR(encoder.GetDistance(), 750, 1.0);
  EXPECT_NEAR(encoder.GetRate(), 0, 0.1);
}
