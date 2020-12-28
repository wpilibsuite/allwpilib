// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "command/MockCommand.h"

using namespace frc;

MockCommand::MockCommand(Subsystem* subsys) : MockCommand() {
  Requires(subsys);
}

MockCommand::MockCommand() {
  m_initializeCount = 0;
  m_executeCount = 0;
  m_isFinishedCount = 0;
  m_hasFinished = false;
  m_endCount = 0;
  m_interruptedCount = 0;
}

bool MockCommand::HasInitialized() {
  return GetInitializeCount() > 0;
}

bool MockCommand::HasEnd() {
  return GetEndCount() > 0;
}

bool MockCommand::HasInterrupted() {
  return GetInterruptedCount() > 0;
}

void MockCommand::Initialize() {
  ++m_initializeCount;
}

void MockCommand::Execute() {
  ++m_executeCount;
}

bool MockCommand::IsFinished() {
  ++m_isFinishedCount;
  return IsHasFinished();
}

void MockCommand::End() {
  ++m_endCount;
}

void MockCommand::Interrupted() {
  ++m_interruptedCount;
}

void MockCommand::ResetCounters() {
  m_initializeCount = 0;
  m_executeCount = 0;
  m_isFinishedCount = 0;
  m_hasFinished = false;
  m_endCount = 0;
  m_interruptedCount = 0;
}
