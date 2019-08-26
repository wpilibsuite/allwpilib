/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/smartdashboard/Sendable.h>

#include <string>

#include <wpi/SmallSet.h>
#include <wpi/SmallVector.h>

#include "Command.h"

namespace frc2 {
/**
 * A Sendable base class for Commands.
 */
class CommandBase : public frc::Sendable, public Command {
 public:
  CommandBase(CommandBase&& other) = default;

  CommandBase(const CommandBase& other);

  /**
   * Adds the specified requirements to the command.
   *
   * @param requirements the requirements to add
   */
  void AddRequirements(std::initializer_list<Subsystem*> requirements);

  void AddRequirements(wpi::SmallSet<Subsystem*, 4> requirements);

  wpi::SmallSet<Subsystem*, 4> GetRequirements() const override;

  void SetName(const wpi::Twine& name) override;

  std::string GetName() const override;

  std::string GetSubsystem() const override;

  void SetSubsystem(const wpi::Twine& subsystem) override;

  void InitSendable(frc::SendableBuilder& builder) override;

 protected:
  CommandBase();
  std::string m_name;
  std::string m_subsystem;
  wpi::SmallSet<Subsystem*, 4> m_requirements;
};
}  // namespace frc2
