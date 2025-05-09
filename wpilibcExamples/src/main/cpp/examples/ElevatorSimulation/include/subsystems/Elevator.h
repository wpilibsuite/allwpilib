// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Encoder.h>
#include <frc/controller/ElevatorFeedforward.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/ElevatorSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/PWMMotorControllerSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <frc/smartdashboard/MechanismRoot2d.h>
#include <units/length.h>

#include "Constants.h"

class Elevator {
 public:
  Elevator();
  void SimulationPeriodic();
  void UpdateTelemetry();
  void ReachGoal(units::meter_t goal);
  void Stop();

 private:
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  frc::DCMotor m_elevatorGearbox = frc::DCMotor::Vex775Pro(4);

  // Standard classes for controlling our elevator
  frc::TrapezoidProfile<units::meters>::Constraints m_constraints{2.45_mps,
                                                                  2.45_mps_sq};
  frc::ProfiledPIDController<units::meters> m_controller{
      Constants::ELEVATOR_P, Constants::ELEVATOR_I, Constants::ELEVATOR_D,
      m_constraints};

  frc::ElevatorFeedforward m_feedforward{
      Constants::ELEVATOR_S, Constants::ELEVATOR_G, Constants::ELEVATOR_V,
      Constants::ELEVATOR_A};
  frc::Encoder m_encoder{Constants::ENCODER_A_CHANNEL,
                         Constants::ENCODER_B_CHANNEL};
  frc::PWMSparkMax m_motor{Constants::MOTOR_PORT};
  frc::sim::PWMMotorControllerSim m_motorSim{m_motor};

  // Simulation classes help us simulate what's going on, including gravity.
  frc::sim::ElevatorSim m_elevatorSim{m_elevatorGearbox,
                                      Constants::ELEVATOR_GEARING,
                                      Constants::CARRIAGE_MASS,
                                      Constants::ELEVATOR_DRUM_RADIUS,
                                      Constants::MIN_ELEVATOR_HEIGHT,
                                      Constants::MAX_ELEVATOR_HEIGHT,
                                      true,
                                      0_m,
                                      {0.01}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

  // Create a Mechanism2d display of an elevator
  frc::Mechanism2d m_mech2d{20, 50};
  frc::MechanismRoot2d* m_elevatorRoot =
      m_mech2d.GetRoot("Elevator Root", 10, 0);
  frc::MechanismLigament2d* m_elevatorMech2d =
      m_elevatorRoot->Append<frc::MechanismLigament2d>(
          "Elevator", m_elevatorSim.GetPosition().value(), 90_deg);
};
