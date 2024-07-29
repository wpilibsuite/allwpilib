// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>

#include "frc/Encoder.h"
#include "frc/RobotController.h"
#include "frc/controller/PIDController.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "frc/motorcontrol/PWMVictorSPX.h"
#include "frc/simulation/BatterySim.h"
#include "frc/simulation/DifferentialDrivetrainSim.h"
#include "frc/simulation/ElevatorSim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/simulation/FlywheelSim.h"
#include "frc/simulation/LinearSystemSim.h"
#include "frc/simulation/PWMSim.h"
#include "frc/simulation/RoboRioSim.h"
#include "frc/system/plant/LinearSystemId.h"

TEST(StateSpaceSimTest, FlywheelSim) {
  const frc::LinearSystem<1, 1, 1> plant =
      frc::LinearSystemId::IdentifyVelocitySystem<units::radian>(
          0.02_V / 1_rad_per_s, 0.01_V / 1_rad_per_s_sq);
  frc::sim::FlywheelSim sim{plant, frc::DCMotor::NEO(2)};
  frc::PIDController controller{0.2, 0.0, 0.0};
  frc::SimpleMotorFeedforward<units::radian> feedforward{
      0_V, 0.02_V / 1_rad_per_s, 0.01_V / 1_rad_per_s_sq};
  frc::Encoder encoder{0, 1};
  frc::sim::EncoderSim encoderSim{encoder};
  frc::PWMVictorSPX motor{0};

  frc::sim::RoboRioSim::ResetData();
  encoderSim.ResetData();

  for (int i = 0; i < 100; i++) {
    // RobotPeriodic runs first
    auto voltageOut = controller.Calculate(encoder.GetRate(), 200.0);
    motor.SetVoltage(units::volt_t{voltageOut} +
                     feedforward.Calculate(200_rad_per_s));

    // Then, SimulationPeriodic runs
    frc::sim::RoboRioSim::SetVInVoltage(
        frc::sim::BatterySim::Calculate({sim.GetCurrentDraw()}));
    sim.SetInput(
        frc::Vectord<1>{motor.Get() * frc::RobotController::GetInputVoltage()});
    sim.Update(20_ms);
    encoderSim.SetRate(sim.GetAngularVelocity().value());
  }

  ASSERT_TRUE(std::abs(200 - encoder.GetRate()) < 0.1);
}
