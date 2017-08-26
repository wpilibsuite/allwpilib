/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <set>

#include <HAL/cpp/priority_mutex.h>

#include "ErrorBase.h"

namespace frc {

class MotorSafety;

class MotorSafetyHelper : public ErrorBase {
 public:
  explicit MotorSafetyHelper(MotorSafety* safeObject);
  ~MotorSafetyHelper();
  void Feed();
  void SetExpiration(double expirationTime);
  double GetExpiration() const;
  bool IsAlive() const;
  void Check();
  void SetSafetyEnabled(bool enabled);
  bool IsSafetyEnabled() const;
  static void CheckMotors();

 private:
  // the expiration time for this object
  double m_expiration;
  // true if motor safety is enabled for this motor
  bool m_enabled;
  // the FPGA clock value when this motor has expired
  double m_stopTime;
  // protect accesses to the state for this object
  mutable hal::priority_recursive_mutex m_syncMutex;
  // the object that is using the helper
  MotorSafety* m_safeObject;
  // List of all existing MotorSafetyHelper objects.
  static std::set<MotorSafetyHelper*> m_helperList;
  // protect accesses to the list of helpers
  static hal::priority_recursive_mutex m_listMutex;
};

}  // namespace frc
