/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/NamedSendable.h"

using namespace frc;

void NamedSendable::SetName(const llvm::Twine&) {}

std::string NamedSendable::GetSubsystem() const { return std::string(); }

void NamedSendable::SetSubsystem(const llvm::Twine&) {}

void NamedSendable::InitSendable(SendableBuilder&) {}
