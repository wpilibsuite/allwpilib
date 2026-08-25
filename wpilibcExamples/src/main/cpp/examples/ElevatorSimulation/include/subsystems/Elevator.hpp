// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/ElevatorFeedforward.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/ProfiledPIDController.hpp"
#include "wpi/simulation/BatterySim.hpp"
#include "wpi/simulation/ElevatorSim.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"
#include "wpi/simulation/RoboRioSim.hpp"
#include "wpi/smartdashboard/Mechanism2d.hpp"
#include "wpi/smartdashboard/MechanismLigament2d.hpp"
#include "wpi/smartdashboard/MechanismRoot2d.hpp"
#include "wpi/units/length.hpp"

class Elevator {
 public:
  Elevator();
  void SimulationPeriodic();
  void UpdateTelemetry();
  void ReachGoal(wpi::units::meters<> goal);
  void Stop();

 private:
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  wpi::math::DCMotor elevatorGearbox = wpi::math::DCMotor::Vex775Pro(4);

  // Standard classes for controlling our elevator
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      2.45_mps, 2.45_mps2};
  wpi::math::ProfiledPIDController<wpi::units::meters_> controller{
      Constants::ELEVATOR_KP, Constants::ELEVATOR_KI, Constants::ELEVATOR_KD,
      constraints};

  wpi::math::ElevatorFeedforward feedforward{
      Constants::ELEVATOR_KS, Constants::ELEVATOR_KG, Constants::ELEVATOR_KV,
      Constants::ELEVATOR_KA};
  wpi::Encoder encoder{Constants::ENCODER_A_CHANNEL,
                       Constants::ENCODER_B_CHANNEL};
  wpi::PWMSparkMax motor{Constants::MOTOR_PORT};
  wpi::sim::PWMMotorControllerSim motorSim{motor};

  // Simulation classes help us simulate what's going on, including gravity.
  wpi::sim::ElevatorSim elevatorSim{elevatorGearbox,
                                    Constants::ELEVATOR_GEARING,
                                    Constants::CARRIAGE_MASS,
                                    Constants::ELEVATOR_DRUM_RADIUS,
                                    Constants::MIN_ELEVATOR_HEIGHT,
                                    Constants::MAX_ELEVATOR_HEIGHT,
                                    true,
                                    0_m,
                                    {0.01}};
  wpi::sim::EncoderSim encoderSim{encoder};

  // Create a Mechanism2d display of an elevator
  wpi::Mechanism2d mech2d{20, 50};
  wpi::MechanismRoot2d* elevatorRoot = mech2d.GetRoot("Elevator Root", 10, 0);
  wpi::MechanismLigament2d* elevatorMech2d =
      elevatorRoot->Append<wpi::MechanismLigament2d>(
          "Elevator", elevatorSim.GetPosition().value(), 90_deg);
};
