/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "MotorSafetyHelper.h"

#include "DriverStation.h"
#include "MotorSafety.h"
#include "Timer.h"
#include "WPIErrors.h"

#include <stdio.h>
#include <sstream>

std::set<MotorSafetyHelper*> MotorSafetyHelper::m_helperList;
priority_recursive_mutex MotorSafetyHelper::m_listMutex;

/**
 * The constructor for a MotorSafetyHelper object.
 * The helper object is constructed for every object that wants to implement the
 * Motor
 * Safety protocol. The helper object has the code to actually do the timing and
 * call the
 * motors Stop() method when the timeout expires. The motor object is expected
 * to call the
 * Feed() method whenever the motors value is updated.
 * @param safeObject a pointer to the motor object implementing MotorSafety.
 * This is used
 * to call the Stop() method on the motor.
 */
MotorSafetyHelper::MotorSafetyHelper(MotorSafety *safeObject)
    : m_safeObject(safeObject) {
  m_enabled = false;
  m_expiration = DEFAULT_SAFETY_EXPIRATION;
  m_stopTime = Timer::GetFPGATimestamp();

  std::lock_guard<priority_recursive_mutex> sync(m_listMutex);
  m_helperList.insert(this);
}

MotorSafetyHelper::~MotorSafetyHelper() {
  std::lock_guard<priority_recursive_mutex> sync(m_listMutex);
  m_helperList.erase(this);
}

/**
 * Feed the motor safety object.
 * Resets the timer on this object that is used to do the timeouts.
 */
void MotorSafetyHelper::Feed() {
  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  m_stopTime = Timer::GetFPGATimestamp() + m_expiration;
}

/**
 * Set the expiration time for the corresponding motor safety object.
 * @param expirationTime The timeout value in seconds.
 */
void MotorSafetyHelper::SetExpiration(float expirationTime) {
  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  m_expiration = expirationTime;
}

/**
 * Retrieve the timeout value for the corresponding motor safety object.
 * @return the timeout value in seconds.
 */
float MotorSafetyHelper::GetExpiration() const {
  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  return m_expiration;
}

/**
 * Determine if the motor is still operating or has timed out.
 * @return a true value if the motor is still operating normally and hasn't
 * timed out.
 */
bool MotorSafetyHelper::IsAlive() const {
  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  return !m_enabled || m_stopTime > Timer::GetFPGATimestamp();
}

/**
 * Check if this motor has exceeded its timeout.
 * This method is called periodically to determine if this motor has exceeded
 * its timeout
 * value. If it has, the stop method is called, and the motor is shut down until
 * its value is
 * updated again.
 */
void MotorSafetyHelper::Check() {
  DriverStation &ds = DriverStation::GetInstance();
  if (!m_enabled || ds.IsDisabled() || ds.IsTest()) return;

  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  if (m_stopTime < Timer::GetFPGATimestamp()) {
    std::ostringstream desc;
    m_safeObject->GetDescription(desc);
    desc <<  "... Output not updated often enough.";
    wpi_setWPIErrorWithContext(Timeout, desc.str().c_str());
    m_safeObject->StopMotor();
  }
}

/**
 * Enable/disable motor safety for this device
 * Turn on and off the motor safety option for this PWM object.
 * @param enabled True if motor safety is enforced for this object
 */
void MotorSafetyHelper::SetSafetyEnabled(bool enabled) {
  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  m_enabled = enabled;
}

/**
 * Return the state of the motor safety enabled flag
 * Return if the motor safety is currently enabled for this devicce.
 * @return True if motor safety is enforced for this device
 */
bool MotorSafetyHelper::IsSafetyEnabled() const {
  std::lock_guard<priority_recursive_mutex> sync(m_syncMutex);
  return m_enabled;
}

/**
 * Check the motors to see if any have timed out.
 * This static  method is called periodically to poll all the motors and stop
 * any that have
 * timed out.
 */
void MotorSafetyHelper::CheckMotors() {
  std::lock_guard<priority_recursive_mutex> sync(m_listMutex);
  for (auto elem : m_helperList) {
    elem->Check();
  }
}
