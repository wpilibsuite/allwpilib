// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/Encoder.h"
#include "frc/RobotController.h"
#include "frc/controller/PIDController.h"
#include "frc/motorcontrol/PWMVictorSPX.h"
#include "frc/simulation/BatterySim.h"
#include "frc/simulation/DCMotorSim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/simulation/RoboRioSim.h"
#include "frc/system/plant/LinearSystemId.h"

TEST(DCMotorSimTest, VoltageSteadyState) {
  frc::DCMotor gearbox = frc::DCMotor::NEO(1);
  auto plant = frc::LinearSystemId::DCMotorSystem(
      frc::DCMotor::NEO(1), units::kilogram_square_meter_t{0.0005}, 1.0);
  frc::sim::DCMotorSim sim{plant, gearbox};

  frc::Encoder encoder{0, 1};
  frc::sim::EncoderSim encoderSim{encoder};
  frc::PWMVictorSPX motor{0};

  frc::sim::RoboRioSim::ResetData();
  encoderSim.ResetData();

  // Spin-up
  for (int i = 0; i < 100; i++) {
    // RobotPeriodic runs first
    motor.SetVoltage(12_V);

    // Then, SimulationPeriodic runs
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInputVoltage(motor.Get() *
                        frc::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR((gearbox.Kv * 12_V).value(), encoder.GetRate(), 0.1);

  // Decay
  for (int i = 0; i < 100; i++) {
    // RobotPeriodic runs first
    motor.SetVoltage(0_V);

    // Then, SimulationPeriodic runs
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInputVoltage(motor.Get() *
                        frc::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR(0, encoder.GetRate(), 0.1);
}

TEST(DCMotorSimTest, PositionFeedbackControl) {
  frc::DCMotor gearbox = frc::DCMotor::NEO(1);
  auto plant = frc::LinearSystemId::DCMotorSystem(
      frc::DCMotor::NEO(1), units::kilogram_square_meter_t{0.0005}, 1.0);
  frc::sim::DCMotorSim sim{plant, gearbox};

  frc::PIDController controller{0.04, 0.0, 0.001};

  frc::Encoder encoder{0, 1};
  frc::sim::EncoderSim encoderSim{encoder};
  frc::PWMVictorSPX motor{0};

  frc::sim::RoboRioSim::ResetData();
  encoderSim.ResetData();

  for (int i = 0; i < 140; i++) {
    // RobotPeriodic runs first
    motor.Set(controller.Calculate(encoder.GetDistance(), 750));

    // Then, SimulationPeriodic runs
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInputVoltage(motor.Get() *
                        frc::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetDistance(sim.GetAngularPosition().value());
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR(encoder.GetDistance(), 750, 1.0);
  EXPECT_NEAR(encoder.GetRate(), 0, 0.1);
}
