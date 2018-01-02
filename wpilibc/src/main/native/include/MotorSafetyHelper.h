/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <set>

#include <support/mutex.h>

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
  // The expiration time for this object
  double m_expiration;

  // True if motor safety is enabled for this motor
  bool m_enabled;

  // The FPGA clock value when this motor has expired
  double m_stopTime;

  // Protect accesses to the state for this object
  mutable wpi::mutex m_thisMutex;

  // The object that is using the helper
  MotorSafety* m_safeObject;

  // List of all existing MotorSafetyHelper objects.
  static std::set<MotorSafetyHelper*> m_helperList;

  // Protect accesses to the list of helpers
  static wpi::mutex m_listMutex;
};

}  // namespace frc
