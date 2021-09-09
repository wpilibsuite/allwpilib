// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>

#include <wpi/SmallSet.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/span.h>

#include "frc2/command/Command.h"

namespace frc2 {
/**
 * A Sendable base class for Commands.
 */
class CommandBase : public Command, public wpi::Sendable {
 public:
  /**
   * Adds the specified requirements to the command.
   *
   * @param requirements the requirements to add
   */
  void AddRequirements(std::initializer_list<Subsystem*> requirements);

  /**
   * Adds the specified requirements to the command.
   *
   * @param requirements the requirements to add
   */
  void AddRequirements(wpi::span<Subsystem* const> requirements);

  void AddRequirements(wpi::SmallSet<Subsystem*, 4> requirements);

  wpi::SmallSet<Subsystem*, 4> GetRequirements() const override;

  /**
   * Sets the name of this Command.
   *
   * @param name name
   */
  void SetName(std::string_view name);

  /**
   * Gets the name of this Command.
   *
   * @return Name
   */
  std::string GetName() const override;

  /**
   * Gets the subsystem name of this Command.
   *
   * @return Subsystem name
   */
  std::string GetSubsystem() const;

  /**
   * Sets the subsystem name of this Command.
   *
   * @param subsystem subsystem name
   */
  void SetSubsystem(std::string_view subsystem);

  void InitSendable(wpi::SendableBuilder& builder) override;

 protected:
  CommandBase();
  wpi::SmallSet<Subsystem*, 4> m_requirements;
};
}  // namespace frc2
