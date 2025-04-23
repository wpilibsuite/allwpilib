// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/Encoder.h"
#include "frc/RobotController.h"
#include "frc/controller/PIDController.h"
#include "frc/motorcontrol/PWMVictorSPX.h"
#include "frc/simulation/BatterySim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/simulation/GearboxSim.h"
#include "frc/simulation/RoboRioSim.h"
#include "frc/system/plant/LinearSystemId.h"

TEST(DCMotorSimTest, VoltageSteadyState) {
  frc::Gearbox gearbox = frc::Gearbox(&frc::NEO, 1, 1.0, 0.0005_kg_sq_m);
  frc::sim::GearboxSim<units::volt> sim{
      gearbox, frc::RobotController::GetBatteryVoltage()};

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
        frc::sim::BatterySim::Calculate({sim.GetCurrent()}));
    sim.SetInput(motor.Get() * frc::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR((gearbox.dcMotor->Kv * 12_V).value(), encoder.GetRate(), 0.1);

  // Decay
  for (int i = 0; i < 100; i++) {
    // RobotPeriodic runs first
    motor.SetVoltage(0_V);

    // Then, SimulationPeriodic runs
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({sim.GetCurrent()}));
    sim.SetInput(motor.Get() * frc::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR(0, encoder.GetRate(), 0.1);
}

TEST(DCMotorSimTest, PositionFeedbackControl) {
  frc::Gearbox gearbox = frc::Gearbox(&frc::NEO, 1, 1.0, 0.0005_kg_sq_m);
  frc::sim::GearboxSim<units::volt> sim{
      gearbox, frc::RobotController::GetBatteryVoltage()};

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
        frc::sim::BatterySim::Calculate({sim.GetCurrent()}));
    sim.SetInput(motor.Get() * frc::RobotController::GetBatteryVoltage());
    sim.Update(20_ms);
    encoderSim.SetDistance(sim.GetAngularPosition().value());
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  EXPECT_NEAR(encoder.GetDistance(), 750, 1.0);
  EXPECT_NEAR(encoder.GetRate(), 0, 0.1);
}
