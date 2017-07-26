/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Servo.h"

#include "LiveWindow/LiveWindow.h"

constexpr double frc::Servo::kMaxServoAngle;
constexpr double frc::Servo::kMinServoAngle;

constexpr double frc::Servo::kDefaultMaxServoPWM;
constexpr double frc::Servo::kDefaultMinServoPWM;

/**
 * @param channel The PWM channel to which the servo is attached. 0-9 are
 *                on-board, 10-19 are on the MXP port
 */
frc::Servo::Servo(int channel) : SafePWM(channel) {
  // Set minimum and maximum PWM values supported by the servo
  SetBounds(kDefaultMaxServoPWM, 0.0, 0.0, 0.0, kDefaultMinServoPWM);

  // Assign defaults for period multiplier for the servo PWM control signal
  SetPeriodMultiplier(kPeriodMultiplier_4X);
}

frc::Servo::~Servo() {
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

/**
 * Set the servo position.
 *
 * Servo values range from 0.0 to 1.0 corresponding to the range of full left to
 * full right.
 *
 * @param value Position from 0.0 to 1.0.
 */
void frc::Servo::Set(double value) { SetPosition(value); }

/**
 * Set the servo to offline.
 *
 * Set the servo raw value to 0 (undriven)
 */
void frc::Servo::SetOffline() { SetRaw(0); }

/**
 * Get the servo position.
 *
 * Servo values range from 0.0 to 1.0 corresponding to the range of full left to
 * full right.
 *
 * @return Position from 0.0 to 1.0.
 */
double frc::Servo::Get() const { return GetPosition(); }

/**
 * Set the servo angle.
 *
 * Assume that the servo angle is linear with respect to the PWM value (big
 * assumption, need to test).
 *
 * Servo angles that are out of the supported range of the servo simply
 * "saturate" in that direction. In other words, if the servo has a range of
 * (X degrees to Y degrees) than angles of less than X result in an angle of
 * X being set and angles of more than Y degrees result in an angle of Y being
 * set.
 *
 * @param degrees The angle in degrees to set the servo.
 */
void frc::Servo::SetAngle(double degrees) {
  if (degrees < kMinServoAngle) {
    degrees = kMinServoAngle;
  } else if (degrees > kMaxServoAngle) {
    degrees = kMaxServoAngle;
  }

  SetPosition((degrees - kMinServoAngle) / GetServoAngleRange());
}

/**
 * Get the servo angle.
 *
 * Assume that the servo angle is linear with respect to the PWM value (big
 * assumption, need to test).
 *
 * @return The angle in degrees to which the servo is set.
 */
double frc::Servo::GetAngle() const {
  return GetPosition() * GetServoAngleRange() + kMinServoAngle;
}

void frc::Servo::ValueChanged(ITable* source, llvm::StringRef key,
                              std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsDouble()) return;
  Set(value->GetDouble());
}

void frc::Servo::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", Get());
  }
}

void frc::Servo::StartLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->AddTableListener("Value", this, true);
  }
}

void frc::Servo::StopLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string frc::Servo::GetSmartDashboardType() const { return "Servo"; }

void frc::Servo::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> frc::Servo::GetTable() const { return m_table; }
