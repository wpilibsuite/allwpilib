// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <units/time.h>

#include "frc/PneumaticsModuleType.h"

namespace frc {
class Solenoid;
class DoubleSolenoid;
class PneumaticsBase {
 public:
  virtual ~PneumaticsBase() = default;

  virtual void SetSolenoids(int mask, int values) = 0;

  virtual int GetSolenoids() const = 0;

  virtual int GetModuleNumber() const = 0;

  virtual int GetSolenoidDisabledList() const = 0;

  virtual void FireOneShot(int index) = 0;

  virtual void SetOneShotDuration(int index, units::second_t duration) = 0;

  virtual bool CheckSolenoidChannel(int channel) const = 0;

  virtual int CheckAndReserveSolenoids(int mask) = 0;

  virtual void UnreserveSolenoids(int mask) = 0;

  virtual Solenoid makeSolenoid(int channel) = 0;
  virtual DoubleSolenoid makeDoubleSolenoid(int forwardChannel, int reverseChannel) = 0;

  static std::shared_ptr<PneumaticsBase> GetForType(int module, PneumaticsModuleType moduleType);
  static int GetDefaultForType(PneumaticsModuleType moduleType);
};
}  // namespace frc
