// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drivetrain.h"

#include <numbers>

#include <frc/Joystick.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <units/length.h>

Drivetrain::Drivetrain() {
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_right.SetInverted(true);

// Encoders may measure differently in the real world and in
// simulation. In this example the robot moves 0.042 barleycorns
// per tick in the real world, but the simulated encoders
// simulate 360 tick encoders. This if statement allows for the
// real robot to handle this difference in devices.
#ifndef SIMULATION
  m_leftEncoder.SetDistancePerPulse(0.042);
  m_rightEncoder.SetDistancePerPulse(0.042);
#else
  // Circumference = diameter * pi. 360 tick simulated encoders.
  m_leftEncoder.SetDistancePerPulse(units::foot_t{4_in}.value() *
                                    std::numbers::pi / 360.0);
  m_rightEncoder.SetDistancePerPulse(units::foot_t{4_in}.value() *
                                     std::numbers::pi / 360.0);
#endif
  SetName("Drivetrain");
  // Let's show everything on the LiveWindow
  AddChild("Front_Left Motor", &m_frontLeft);
  AddChild("Rear Left Motor", &m_rearLeft);
  AddChild("Front Right Motor", &m_frontRight);
  AddChild("Rear Right Motor", &m_rearRight);
  AddChild("Left Encoder", &m_leftEncoder);
  AddChild("Right Encoder", &m_rightEncoder);
  AddChild("Rangefinder", &m_rangefinder);
  AddChild("Gyro", &m_gyro);
}

void Drivetrain::Log() {
  frc::SmartDashboard::PutNumber("Left Distance", m_leftEncoder.GetDistance());
  frc::SmartDashboard::PutNumber("Right Distance",
                                 m_rightEncoder.GetDistance());
  frc::SmartDashboard::PutNumber("Left Speed", m_leftEncoder.GetRate());
  frc::SmartDashboard::PutNumber("Right Speed", m_rightEncoder.GetRate());
  frc::SmartDashboard::PutNumber("Gyro", m_gyro.GetAngle());
}

void Drivetrain::Drive(double left, double right) {
  m_robotDrive.TankDrive(left, right);
}

double Drivetrain::GetHeading() const {
  return m_gyro.GetAngle();
}

void Drivetrain::Reset() {
  m_gyro.Reset();
  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
}

double Drivetrain::GetDistance() const {
  return (m_leftEncoder.GetDistance() + m_rightEncoder.GetDistance()) / 2.0;
}

double Drivetrain::GetDistanceToObstacle() const {
  // Really meters in simulation since it's a rangefinder...
  return m_rangefinder.GetAverageVoltage();
}

void Drivetrain::Periodic() {
  Log();
}
