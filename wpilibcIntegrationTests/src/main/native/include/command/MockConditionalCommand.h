/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "command/MockCommand.h"
#include "frc/commands/ConditionalCommand.h"

namespace frc {

class MockConditionalCommand : public ConditionalCommand {
 public:
  MockConditionalCommand(MockCommand* onTrue, MockCommand* onFalse);
  void SetCondition(bool condition);
  int32_t GetInitializeCount() { return m_initializeCount; }
  bool HasInitialized();

  int32_t GetExecuteCount() { return m_executeCount; }
  int32_t GetIsFinishedCount() { return m_isFinishedCount; }
  int32_t GetEndCount() { return m_endCount; }
  bool HasEnd();

  int32_t GetInterruptedCount() { return m_interruptedCount; }
  bool HasInterrupted();
  void ResetCounters();

 protected:
  bool Condition() override;
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;

 private:
  bool m_condition = false;
  int32_t m_initializeCount;
  int32_t m_executeCount;
  int32_t m_isFinishedCount;
  int32_t m_endCount;
  int32_t m_interruptedCount;
};

}  // namespace frc
