// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/ElevatorFeedforward.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/trajectory/ExponentialProfile.hpp"
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
  void ReachGoal(wpi::units::meter_t goal);
  void Reset();
  void Stop();

 private:
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  wpi::math::DCMotor m_elevatorGearbox = wpi::math::DCMotor::NEO(2);

  // Standard classes for controlling our elevator
  wpi::math::ExponentialProfile<wpi::units::meters,
                                wpi::units::volts>::Constraints m_constraints{
      Constants::kElevatorMaxV, Constants::kElevatorkV, Constants::kElevatorkA};
  wpi::math::ExponentialProfile<wpi::units::meters, wpi::units::volts>
      m_profile{m_constraints};
  wpi::math::ExponentialProfile<wpi::units::meters, wpi::units::volts>::State
      m_setpoint;

  wpi::math::PIDController m_controller{
      Constants::kElevatorKp, Constants::kElevatorKi, Constants::kElevatorKd};

  wpi::math::ElevatorFeedforward m_feedforward{
      Constants::kElevatorkS, Constants::kElevatorkG, Constants::kElevatorkV,
      Constants::kElevatorkA};
  wpi::Encoder m_encoder{Constants::kEncoderAChannel,
                         Constants::kEncoderBChannel};
  wpi::PWMSparkMax m_motor{Constants::kMotorPort};
  wpi::sim::PWMMotorControllerSim m_motorSim{m_motor};

  // Simulation classes help us simulate what's going on, including gravity.
  wpi::sim::ElevatorSim m_elevatorSim{m_elevatorGearbox,
                                      Constants::kElevatorGearing,
                                      Constants::kCarriageMass,
                                      Constants::kElevatorDrumRadius,
                                      Constants::kMinElevatorHeight,
                                      Constants::kMaxElevatorHeight,
                                      true,
                                      0_m,
                                      {0.005}};
  wpi::sim::EncoderSim m_encoderSim{m_encoder};

  // Create a Mechanism2d display of an elevator
  wpi::Mechanism2d m_mech2d{10_in / 1_m, 51_in / 1_m};
  wpi::MechanismRoot2d* m_elevatorRoot =
      m_mech2d.GetRoot("Elevator Root", 5_in / 1_m, 0.5_in / 1_m);
  wpi::MechanismLigament2d* m_elevatorMech2d =
      m_elevatorRoot->Append<wpi::MechanismLigament2d>(
          "Elevator", m_elevatorSim.GetPosition().value(), 90_deg);
};
