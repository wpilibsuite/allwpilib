// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/DigitalInput.h>
#include <frc/DoubleSolenoid.h>
#include <frc/Solenoid.h>
#include <frc/commands/Subsystem.h>

/**
 * The Shooter subsystem handles shooting. The mechanism for shooting is
 * slightly complicated because it has to pneumatic cylinders for shooting, and
 * a third latch to allow the pressure to partially build up and reduce the
 * effect of the airflow. For shorter shots, when full power isn't needed, only
 * one cylinder fires.
 *
 * NOTE: Simulation currently approximates this as as single pneumatic cylinder
 * and ignores the latch.
 */
class Shooter : public frc::Subsystem {
 public:
  Shooter();
  void InitDefaultCommand() override;

  /**
   * Extend both solenoids to shoot.
   */
  void ExtendBoth();

  /**
   * Retract both solenoids to prepare to shoot.
   */
  void RetractBoth();

  /**
   * Extend solenoid 1 to shoot.
   */
  void Extend1();

  /**
   * Retract solenoid 1 to prepare to shoot.
   */
  void Retract1();

  /**
   * Extend solenoid 2 to shoot.
   */
  void Extend2();

  /**
   * Retract solenoid 2 to prepare to shoot.
   */
  void Retract2();

  /**
   * Turns off the piston1 double solenoid. This won't actuate anything
   * because double solenoids preserve their state when turned off. This
   * should be called in order to reduce the amount of time that the coils
   * are
   * powered.
   */
  void Off1();

  /**
   * Turns off the piston1 double solenoid. This won't actuate anything
   * because double solenoids preserve their state when turned off. This
   * should be called in order to reduce the amount of time that the coils
   * are
   * powered.
   */
  void Off2();

  /**
   * Release the latch so that we can shoot
   */
  void Unlatch();

  /**
   * Latch so that pressure can build up and we aren't limited by air
   * flow.
   */
  void Latch();

  /**
   * Toggles the latch postions
   */
  void ToggleLatchPosition();

  /**
   * @return Whether or not piston 1 is fully extended.
   */
  bool Piston1IsExtended();

  /**
   * @return Whether or not piston 1 is fully retracted.
   */
  bool Piston1IsRetracted();

  /**
   * Turns off all double solenoids. Double solenoids hold their position
   * when
   * they are turned off. We should turn them off whenever possible to
   * extend
   * the life of the coils
   */
  void OffBoth();

  /**
   * @return Whether or not the goal is hot as read by the banner sensor
   */
  bool GoalIsHot();

 private:
  // Devices
  frc::DoubleSolenoid m_piston1{frc::PneumaticsModuleType::CTREPCM, 3, 4};
  frc::DoubleSolenoid m_piston2{frc::PneumaticsModuleType::CTREPCM, 5, 6};
  frc::Solenoid m_latchPiston{1, frc::PneumaticsModuleType::CTREPCM, 2};
  frc::DigitalInput m_piston1ReedSwitchFront{9};
  frc::DigitalInput m_piston1ReedSwitchBack{11};
  frc::DigitalInput m_hotGoalSensor{
      7};  // NOTE: Currently ignored in simulation
};
