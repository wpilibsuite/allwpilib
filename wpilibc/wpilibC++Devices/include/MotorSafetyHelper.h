/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "HAL/cpp/priority_mutex.h"

#include <set>

class MotorSafety;

class MotorSafetyHelper : public ErrorBase {
 public:
  MotorSafetyHelper(MotorSafety *safeObject);
  ~MotorSafetyHelper();
  void Feed();
  void SetExpiration(float expirationTime);
  float GetExpiration() const;
  bool IsAlive() const;
  void Check();
  void SetSafetyEnabled(bool enabled);
  bool IsSafetyEnabled() const;
  static void CheckMotors();

 private:
  double m_expiration;  // the expiration time for this object
  bool m_enabled;       // true if motor safety is enabled for this motor
  double m_stopTime;    // the FPGA clock value when this motor has expired
  mutable priority_recursive_mutex
      m_syncMutex;            // protect accesses to the state for this object
  MotorSafety *m_safeObject;  // the object that is using the helper
  // List of all existing MotorSafetyHelper objects.
  static std::set<MotorSafetyHelper*> m_helperList;
  static priority_recursive_mutex
      m_listMutex;  // protect accesses to the list of helpers
};
