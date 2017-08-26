/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <thread>

#include <HAL/HAL.h>
#include <llvm/raw_ostream.h>

#include "Base.h"

namespace frc {

class DriverStation;

#define START_ROBOT_CLASS(_ClassName_)                                  \
  int main() {                                                          \
    if (!HAL_Initialize(500, 0)) {                                      \
      llvm::errs() << "FATAL ERROR: HAL could not be initialized\n";    \
      return -1;                                                        \
    }                                                                   \
    HAL_Report(HALUsageReporting::kResourceType_Language,               \
               HALUsageReporting::kLanguage_CPlusPlus);                 \
    llvm::outs() << "\n********** Robot program starting **********\n"; \
    static _ClassName_ robot;                                           \
    robot.StartCompetition();                                           \
  }

/**
 * Implement a Robot Program framework.
 * The RobotBase class is intended to be subclassed by a user creating a robot
 * program. Overridden Autonomous() and OperatorControl() methods are called at
 * the appropriate time as the match proceeds. In the current implementation,
 * the Autonomous code will run to completion before the OperatorControl code
 * could start. In the future the Autonomous code might be spawned as a task,
 * then killed at the end of the Autonomous period.
 */
class RobotBase {
 public:
  bool IsEnabled() const;
  bool IsDisabled() const;
  bool IsAutonomous() const;
  bool IsOperatorControl() const;
  bool IsTest() const;
  bool IsNewDataAvailable() const;
  static std::thread::id GetThreadId();
  virtual void StartCompetition() = 0;

 protected:
  RobotBase();
  virtual ~RobotBase() = default;

  RobotBase(const RobotBase&) = delete;
  RobotBase& operator=(const RobotBase&) = delete;

  DriverStation& m_ds;

  static std::thread::id m_threadId;
};

}  // namespace frc
