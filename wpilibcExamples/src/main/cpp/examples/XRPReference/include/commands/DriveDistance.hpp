// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "subsystems/Drivetrain.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/units/length.hpp"

class DriveDistance
    : public wpi::cmd::CommandHelper<wpi::cmd::Command, DriveDistance> {
 public:
  /**
   * Creates a new DriveDistance. This command will drive your your robot for a
   * desired distance at a desired velocity.
   *
   * @param velocity The velocity at which the robot will drive
   * @param distance The distance the robot will drive
   * @param drive The drivetrain subsystem on which this command will run
   */
  DriveDistance(double velocity, wpi::units::meter_t distance,
                Drivetrain* drive)
      : m_velocity(velocity), m_distance(distance), m_drive(drive) {
    AddRequirements(m_drive);
  }

  void Initialize() override;
  void Execute() override;
  void End(bool interrupted) override;
  bool IsFinished() override;

 private:
  double m_velocity;
  wpi::units::meter_t m_distance;
  Drivetrain* m_drive;
};
