// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Encoder.h>
#include <frc/controller/ArmFeedforward.h>
#include <frc/controller/PIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/BatterySim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/simulation/RoboRioSim.h>
#include <frc/simulation/SingleJointedArmSim.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <frc/smartdashboard/MechanismRoot2d.h>
#include <units/length.h>

#include "Constants.h"

class Arm {
 public:
  Arm();
  void SimulationPeriodic();
  void LoadPreferences();
  void ReachSetpoint();
  void Stop();

 private:
  // The P gain for the PID controller that drives this arm.
  double m_armKp = kDefaultArmKp;
  units::degree_t m_armSetpoint = kDefaultArmSetpoint;

  // The arm gearbox represents a gearbox containing two Vex 775pro motors.
  frc::DCMotor m_armGearbox = frc::DCMotor::Vex775Pro(2);

  // Standard classes for controlling our arm
  frc2::PIDController m_controller{m_armKp, 0, 0};
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};
  frc::PWMSparkMax m_motor{kMotorPort};

  // Simulation classes help us simulate what's going on, including gravity.
  // This sim represents an arm with 2 775s, a 600:1 reduction, a mass of 5kg,
  // 30in overall arm length, range of motion in [-75, 255] degrees, and noise
  // with a standard deviation of 1 encoder tick.
  frc::sim::SingleJointedArmSim m_armSim{
      m_armGearbox,
      kArmReduction,
      frc::sim::SingleJointedArmSim::EstimateMOI(kArmLength, kArmMass),
      kArmLength,
      kMinAngle,
      kMaxAngle,
      true,
      {kArmEncoderDistPerPulse}};
  frc::sim::EncoderSim m_encoderSim{m_encoder};

  // Create a Mechanism2d display of an Arm
  frc::Mechanism2d m_mech2d{60, 60};
  frc::MechanismRoot2d* m_armBase = m_mech2d.GetRoot("ArmBase", 30, 30);
  frc::MechanismLigament2d* m_armTower =
      m_armBase->Append<frc::MechanismLigament2d>(
          "Arm Tower", 30, -90_deg, 6, frc::Color8Bit{frc::Color::kBlue});
  frc::MechanismLigament2d* m_arm = m_armBase->Append<frc::MechanismLigament2d>(
      "Arm", 30, m_armSim.GetAngle(), 6, frc::Color8Bit{frc::Color::kYellow});
};
