/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/livewindow/LiveWindowSendable.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

std::string LiveWindowSendable::GetName() const { return std::string(); }

void LiveWindowSendable::SetName(const wpi::Twine&) {}

std::string LiveWindowSendable::GetSubsystem() const { return std::string(); }

void LiveWindowSendable::SetSubsystem(const wpi::Twine&) {}

void LiveWindowSendable::InitSendable(SendableBuilder& builder) {
  builder.SetUpdateTable([=]() { UpdateTable(); });
}
