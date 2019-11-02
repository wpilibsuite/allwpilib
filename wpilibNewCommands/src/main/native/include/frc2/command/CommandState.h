/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

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
  double TimeSinceInitialized() const;

 private:
  double m_startTime = -1;
  bool m_interruptible;

  void StartTiming();
  void StartRunning();
};
}  // namespace frc2
