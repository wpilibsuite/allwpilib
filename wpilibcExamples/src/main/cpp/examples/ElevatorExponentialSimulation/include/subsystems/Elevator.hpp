// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Encoder.h>
#include <frc/controller/ElevatorFeedforward.h>
#include <frc/controller/PIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/ElevatorSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/PWMMotorControllerSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <frc/smartdashboard/MechanismRoot2d.h>
#include <frc/trajectory/ExponentialProfile.h>
#include <units/length.h>

#include "Constants.h"

class Elevator {
 public:
  Elevator();
  void SimulationPeriodic();
  void UpdateTelemetry();
  void ReachGoal(units::meter_t goal);
  void Reset();
  void Stop();

 private:
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  frc::DCMotor m_elevatorGearbox = frc::DCMotor::NEO(2);

  // Standard classes for controlling our elevator
  frc::ExponentialProfile<units::meters, units::volts>::Constraints
      m_constraints{Constants::kElevatorMaxV, Constants::kElevatorkV,
                    Constants::kElevatorkA};
  frc::ExponentialProfile<units::meters, units::volts> m_profile{m_constraints};
  frc::ExponentialProfile<units::meters, units::volts>::State m_setpoint;

  frc::PIDController m_controller{
      Constants::kElevatorKp, Constants::kElevatorKi, Constants::kElevatorKd};

  frc::ElevatorFeedforward m_feedforward{
      Constants::kElevatorkS, Constants::kElevatorkG, Constants::kElevatorkV,
      Constants::kElevatorkA};
  frc::Encoder m_encoder{Constants::kEncoderAChannel,
                         Constants::kEncoderBChannel};
  frc::PWMSparkMax m_motor{Constants::kMotorPort};
  frc::sim::PWMMotorControllerSim m_motorSim{m_motor};

  // Simulation classes help us simulate what's going on, including gravity.
  frc::sim::ElevatorSim m_elevatorSim{m_elevatorGearbox,
                                      Constants::kElevatorGearing,
                                      Constants::kCarriageMass,
                                      Constants::kElevatorDrumRadius,
                                      Constants::kMinElevatorHeight,
                                      Constants::kMaxElevatorHeight,
                                      true,
                                      0_m,
                                      {0.005}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

  // Create a Mechanism2d display of an elevator
  frc::Mechanism2d m_mech2d{10_in / 1_m, 51_in / 1_m};
  frc::MechanismRoot2d* m_elevatorRoot =
      m_mech2d.GetRoot("Elevator Root", 5_in / 1_m, 0.5_in / 1_m);
  frc::MechanismLigament2d* m_elevatorMech2d =
      m_elevatorRoot->Append<frc::MechanismLigament2d>(
          "Elevator", m_elevatorSim.GetPosition().value(), 90_deg);
};
