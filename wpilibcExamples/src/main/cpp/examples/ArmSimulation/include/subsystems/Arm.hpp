// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/ArmFeedforward.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/simulation/BatterySim.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/simulation/PWMSim.hpp"
#include "wpi/simulation/RoboRioSim.hpp"
#include "wpi/simulation/SingleJointedArmSim.hpp"
#include "wpi/smartdashboard/Mechanism2d.hpp"
#include "wpi/smartdashboard/MechanismLigament2d.hpp"
#include "wpi/smartdashboard/MechanismRoot2d.hpp"
#include "wpi/units/length.hpp"

class Arm {
 public:
  Arm();
  void SimulationPeriodic();
  void LoadPreferences();
  void ReachSetpoint();
  void Stop();

 private:
  // The P gain for the PID controller that drives this arm.
  double armKp = kDefaultArmKp;
  wpi::units::degree_t armSetpoint = kDefaultArmSetpoint;

  // The arm gearbox represents a gearbox containing two Vex 775pro motors.
  wpi::math::DCMotor armGearbox = wpi::math::DCMotor::Vex775Pro(2);

  // Standard classes for controlling our arm
  wpi::math::PIDController controller{armKp, 0, 0};
  wpi::Encoder encoder{kEncoderAChannel, kEncoderBChannel};
  wpi::PWMSparkMax motor{kMotorPort};

  // Simulation classes help us simulate what's going on, including gravity.
  // This sim represents an arm with 2 775s, a 600:1 reduction, a mass of 5kg,
  // 30in overall arm length, range of motion in [-75, 255] degrees, and noise
  // with a standard deviation of 1 encoder tick.
  wpi::sim::SingleJointedArmSim armSim{
      armGearbox,
      kArmReduction,
      wpi::sim::SingleJointedArmSim::EstimateMOI(kArmLength, kArmMass),
      kArmLength,
      kMinAngle,
      kMaxAngle,
      true,
      0_deg,
      {kArmEncoderDistPerPulse}};
  wpi::sim::EncoderSim encoderSim{encoder};

  // Create a Mechanism2d display of an Arm
  wpi::Mechanism2d mech2d{60, 60};
  wpi::MechanismRoot2d* armBase = mech2d.GetRoot("ArmBase", 30, 30);
  wpi::MechanismLigament2d* armTower =
      armBase->Append<wpi::MechanismLigament2d>(
          "Arm Tower", 30, -90_deg, 6,
          wpi::util::Color8Bit{wpi::util::Color::BLUE});
  wpi::MechanismLigament2d* arm = armBase->Append<wpi::MechanismLigament2d>(
      "Arm", 30, armSim.GetAngle(), 6,
      wpi::util::Color8Bit{wpi::util::Color::YELLOW});
};
