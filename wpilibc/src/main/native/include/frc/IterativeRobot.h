/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
 */
class IterativeRobot : public IterativeRobotBase {
 public:
  WPI_DEPRECATED(
      "Use TimedRobot instead. It's a drop-in replacement that provides more "
      "regular execution periods.")
  IterativeRobot();
  virtual ~IterativeRobot() = default;

  /**
   * Provide an alternate "main loop" via StartCompetition().
   *
   * This specific StartCompetition() implements "main loop" behaviour synced
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
