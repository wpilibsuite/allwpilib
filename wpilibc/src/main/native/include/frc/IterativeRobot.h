// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>

#include "frc/IterativeRobotBase.h"

namespace frc {

/**
 * IterativeRobot implements the IterativeRobotBase robot program framework.
 *
 * The IterativeRobot class is intended to be subclassed by a user creating a
 * robot program.
 *
 * Periodic() functions from the base class are called each time a new packet is
 * received from the driver station.
 *
 * @deprecated Use TimedRobot instead. It's a drop-in replacement that provides
 * more regular execution periods.
 */
class IterativeRobot : public IterativeRobotBase {
 public:
  WPI_DEPRECATED(
      "Use TimedRobot instead. It's a drop-in replacement that provides more "
      "regular execution periods.")
  IterativeRobot();
  ~IterativeRobot() override = default;

  /**
   * Provide an alternate "main loop" via StartCompetition().
   *
   * This specific StartCompetition() implements "main loop" behavior synced
   * with the DS packets.
   */
  void StartCompetition() override;

  /**
   * Ends the main loop in StartCompetition().
   */
  void EndCompetition() override;

 private:
  std::atomic<bool> m_exit{false};
};

}  // namespace frc
