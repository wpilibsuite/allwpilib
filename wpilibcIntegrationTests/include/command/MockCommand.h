/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

class MockCommand : public Command {
 private:
  int m_initializeCount;
  int m_executeCount;
  int m_isFinishedCount;
  bool m_hasFinished;
  int m_endCount;
  int m_interruptedCount;

 protected:
  virtual void Initialize();
  virtual void Execute();
  virtual bool IsFinished();
  virtual void End();
  virtual void Interrupted();

 public:
  MockCommand();
  int GetInitializeCount() { return m_initializeCount; }
  bool HasInitialized();

  int GetExecuteCount() { return m_executeCount; }
  int GetIsFinishedCount() { return m_isFinishedCount; }
  bool IsHasFinished() { return m_hasFinished; }
  void SetHasFinished(bool hasFinished) { m_hasFinished = hasFinished; }
  int GetEndCount() { return m_endCount; }
  bool HasEnd();

  int GetInterruptedCount() { return m_interruptedCount; }
  bool HasInterrupted();
};
