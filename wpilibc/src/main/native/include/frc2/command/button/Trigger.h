/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/smartdashboard/SendableBase.h>

#include <atomic>
#include <utility>

namespace frc2 {
class Command;
class Trigger {
 public:
  explicit Trigger(std::function<bool()> isActive)
      : m_isActive{std::move(isActive)} {}

  Trigger() {
    m_isActive = [] { return false; };
  }

  Trigger(const Trigger& other);

  virtual bool Get() const { return m_isActive(); }

  Trigger WhenActive(Command* command, bool interruptible);
  Trigger WhenActive(Command* command) {
    WhenActive(command, true);
    return *this;
  }
  Trigger WhenActive(std::function<void()> toRun);
  Trigger WhileActiveContinous(Command* command, bool interruptible);
  Trigger WhileActiveContinous(Command* command) {
    WhileActiveContinous(command, true);
    return *this;
  }
  Trigger WhileActiveContinous(std::function<void()> toRun);
  Trigger WhileActiveOnce(Command* command, bool interruptible);
  Trigger WhileActiveOnce(Command* command) {
    WhileActiveOnce(command, true);
    return *this;
  }
  Trigger WhenInactive(Command* command, bool interruptible);
  Trigger WhenInactive(Command* command) {
    WhenInactive(command, true);
    return *this;
  }
  Trigger WhenInactive(std::function<void()> toRun);
  Trigger ToggleWhenActive(Command* command, bool interruptible);
  Trigger ToggleWhenActive(Command* command) {
    ToggleWhenActive(command, true);
    return *this;
  }
  Trigger CancelWhenActive(Command* command);

  Trigger operator&&(Trigger rhs) {
    return Trigger([*this, rhs] { return Get() && rhs.Get(); });
  }

  Trigger operator||(Trigger rhs) {
    return Trigger([*this, rhs] { return Get() || rhs.Get(); });
  }

  Trigger operator!() {
    return Trigger([*this] { return !Get(); });
  }

 private:
  std::function<bool()> m_isActive;
};
}  // namespace frc2
