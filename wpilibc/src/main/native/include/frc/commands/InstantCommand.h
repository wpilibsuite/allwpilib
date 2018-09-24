/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include <wpi/Twine.h>

#include "frc/commands/Command.h"
#include "frc/commands/Subsystem.h"

namespace frc {

/**
 * This command will execute once, then finish immediately afterward.
 *
 * Subclassing InstantCommand is shorthand for returning true from IsFinished().
 */
class InstantCommand : public Command {
 public:
  /**
   * Creates a new InstantCommand with the given name.
   *
   * @param name The name for this command
   */
  explicit InstantCommand(const wpi::Twine& name);

  /**
   * Creates a new InstantCommand with the given requirement.
   *
   * @param subsystem The subsystem that the command requires
   */
  explicit InstantCommand(Subsystem& subsystem);

  /**
   * Creates a new InstantCommand with the given name.
   *
   * @param name      The name for this command
   * @param subsystem The subsystem that the command requires
   */
  InstantCommand(const wpi::Twine& name, Subsystem& subsystem);

  /**
   * Create a command that calls the given function when run.
   *
   * @param func The function to run when Initialize() is run.
   */
  explicit InstantCommand(std::function<void()> func);

  /**
   * Create a command that calls the given function when run.
   *
   * @param subsystem The subsystems that this command runs on.
   * @param func      The function to run when Initialize() is run.
   */
  InstantCommand(Subsystem& subsystem, std::function<void()> func);

  /**
   * Create a command that calls the given function when run.
   *
   * @param name   The name of the command.
   * @param func   The function to run when Initialize() is run.
   */
  InstantCommand(const wpi::Twine& name, std::function<void()> func);

  /**
   * Create a command that calls the given function when run.
   *
   * @param name      The name of the command.
   * @param subsystem The subsystems that this command runs on.
   * @param func      The function to run when Initialize() is run.
   */
  InstantCommand(const wpi::Twine& name, Subsystem& subsystem,
                 std::function<void()> func);

  InstantCommand() = default;
  virtual ~InstantCommand() = default;

  InstantCommand(InstantCommand&&) = default;
  InstantCommand& operator=(InstantCommand&&) = default;

 protected:
  std::function<void()> m_func = nullptr;
  void _Initialize() override;
  bool IsFinished() override;
};

}  // namespace frc
