// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/time.h>

namespace frc2 {
/**
 * Class that holds scheduling state for a command.  Used internally by the
 * CommandScheduler
 */
class CommandState final {
 public:
  CommandState() = default;

  explicit CommandState(bool interruptible);

  bool IsInterruptible() const { return m_interruptible; }

  // The time since this command was initialized.
  units::second_t TimeSinceInitialized() const;

 private:
  units::second_t m_startTime = -1_s;
  bool m_interruptible;

  void StartTiming();
  void StartRunning();
};
}  // namespace frc2
