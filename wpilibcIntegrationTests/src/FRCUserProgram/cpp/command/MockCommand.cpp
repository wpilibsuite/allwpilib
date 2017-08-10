/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "command/MockCommand.h"

using namespace frc;

MockCommand::MockCommand() {
  m_initializeCount = 0;
  m_executeCount = 0;
  m_isFinishedCount = 0;
  m_hasFinished = false;
  m_endCount = 0;
  m_interruptedCount = 0;
}

bool MockCommand::HasInitialized() { return GetInitializeCount() > 0; }

bool MockCommand::HasEnd() { return GetEndCount() > 0; }

bool MockCommand::HasInterrupted() { return GetInterruptedCount() > 0; }

void MockCommand::Initialize() { ++m_initializeCount; }

void MockCommand::Execute() { ++m_executeCount; }

bool MockCommand::IsFinished() {
  ++m_isFinishedCount;
  return IsHasFinished();
}

void MockCommand::End() { ++m_endCount; }

void MockCommand::Interrupted() { ++m_interruptedCount; }
