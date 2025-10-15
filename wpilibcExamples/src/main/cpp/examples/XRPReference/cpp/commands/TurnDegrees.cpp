// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TurnDegrees.hpp"

#include <numbers>

#include <wpi/units/math.hpp>

void TurnDegrees::Initialize() {
  // Set motors to stop, read encoder values for starting point
  m_drive->ArcadeDrive(0, 0);
  m_drive->ResetEncoders();
}

void TurnDegrees::Execute() {
  m_drive->ArcadeDrive(0, m_speed);
}

void TurnDegrees::End(bool interrupted) {
  m_drive->ArcadeDrive(0, 0);
}

bool TurnDegrees::IsFinished() {
  // Need to convert distance travelled to degrees. The Standard XRP Chassis
  // found here https://www.sparkfun.com/products/22230, has a
  // wheel placement diameter (163 mm) - width of the wheel (8 mm) = 155 mm
  // or 6.102 inches. We then take into consideration the width of the tires.
  static auto inchPerDegree = (6.102_in * std::numbers::pi) / 360_deg;

  // Compare distance traveled from start to distance based on degree turn.
  return GetAverageTurningDistance() >= inchPerDegree * m_angle;
}

units::meter_t TurnDegrees::GetAverageTurningDistance() {
  auto l = units::math::abs(m_drive->GetLeftDistance());
  auto r = units::math::abs(m_drive->GetRightDistance());
  return (l + r) / 2;
}
