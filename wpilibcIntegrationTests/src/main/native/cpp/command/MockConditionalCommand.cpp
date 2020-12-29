// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "command/MockConditionalCommand.h"

using namespace frc;

MockConditionalCommand::MockConditionalCommand(MockCommand* onTrue,
                                               MockCommand* onFalse)
    : ConditionalCommand(onTrue, onFalse) {
  m_initializeCount = 0;
  m_executeCount = 0;
  m_isFinishedCount = 0;
  m_endCount = 0;
  m_interruptedCount = 0;
}

void MockConditionalCommand::SetCondition(bool condition) {
  m_condition = condition;
}

bool MockConditionalCommand::Condition() {
  return m_condition;
}

bool MockConditionalCommand::HasInitialized() {
  return GetInitializeCount() > 0;
}

bool MockConditionalCommand::HasEnd() {
  return GetEndCount() > 0;
}

bool MockConditionalCommand::HasInterrupted() {
  return GetInterruptedCount() > 0;
}

void MockConditionalCommand::Initialize() {
  ++m_initializeCount;
}

void MockConditionalCommand::Execute() {
  ++m_executeCount;
}

bool MockConditionalCommand::IsFinished() {
  ++m_isFinishedCount;
  return ConditionalCommand::IsFinished();
}

void MockConditionalCommand::End() {
  ++m_endCount;
}

void MockConditionalCommand::Interrupted() {
  ++m_interruptedCount;
}

void MockConditionalCommand::ResetCounters() {
  m_initializeCount = 0;
  m_executeCount = 0;
  m_isFinishedCount = 0;
  m_endCount = 0;
  m_interruptedCount = 0;
}
