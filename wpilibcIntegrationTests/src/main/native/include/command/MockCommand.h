// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/commands/Command.h"

namespace frc {

class MockCommand : public Command {
 public:
  explicit MockCommand(Subsystem*);
  MockCommand();
  int32_t GetInitializeCount() { return m_initializeCount; }
  bool HasInitialized();

  int32_t GetExecuteCount() { return m_executeCount; }
  int32_t GetIsFinishedCount() { return m_isFinishedCount; }
  bool IsHasFinished() { return m_hasFinished; }
  void SetHasFinished(bool hasFinished) { m_hasFinished = hasFinished; }
  int32_t GetEndCount() { return m_endCount; }
  bool HasEnd();

  int32_t GetInterruptedCount() { return m_interruptedCount; }
  bool HasInterrupted();
  void ResetCounters();

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;

 private:
  int32_t m_initializeCount;
  int32_t m_executeCount;
  int32_t m_isFinishedCount;
  bool m_hasFinished;
  int32_t m_endCount;
  int32_t m_interruptedCount;
};

}  // namespace frc
