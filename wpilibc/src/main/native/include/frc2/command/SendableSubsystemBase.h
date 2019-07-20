/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/smartdashboard/Sendable.h>

#include <string>

#include "Subsystem.h"

namespace frc2 {
/**
 * A base for subsystems that handles registration in the constructor, and
 * provides a more intuitive method for setting the default command.
 */
class SendableSubsystemBase : public Subsystem, public frc::Sendable {
 public:
  void InitSendable(frc::SendableBuilder& builder) override;
  std::string GetName() const override;
  void SetName(const wpi::Twine& name) override;
  std::string GetSubsystem() const override;
  void SetSubsystem(const wpi::Twine& name) override;
  void AddChild(std::string name, frc::Sendable* child);

 protected:
  SendableSubsystemBase();
  std::string m_name;
};
}  // namespace frc2
