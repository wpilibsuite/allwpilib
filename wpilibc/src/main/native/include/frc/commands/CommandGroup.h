/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <wpi/Twine.h>

#include "frc/commands/Command.h"
#include "frc/commands/CommandGroupEntry.h"

namespace frc {

/**
 * A CommandGroup is a list of commands which are executed in sequence.
 *
 * Commands in a CommandGroup are added using the AddSequential() method and are
 * called sequentially. CommandGroups are themselves Commands and can be given
 * to other CommandGroups.
 *
 * CommandGroups will carry all of the requirements of their Command
 * subcommands. Additional requirements can be specified by calling Requires()
 * normally in the constructor.
 *
 * CommandGroups can also execute commands in parallel, simply by adding them
 * using AddParallel().
 *
 * @see Command
 * @see Subsystem
 */
class CommandGroup : public Command {
 public:
  CommandGroup() = default;

  /**
   * Creates a new CommandGroup with the given name.
   *
   * @param name The name for this command group
   */
  explicit CommandGroup(const wpi::Twine& name);

  virtual ~CommandGroup() = default;

  CommandGroup(CommandGroup&&) = default;
  CommandGroup& operator=(CommandGroup&&) = default;

  /**
   * Adds a new Command to the group. The Command will be started after all the
   * previously added Commands.
   *
   * Note that any requirements the given Command has will be added to the
   * group. For this reason, a Command's requirements can not be changed after
   * being added to a group.
   *
   * It is recommended that this method be called in the constructor.
   *
   * @param command The Command to be added
   */
  void AddSequential(Command* command);

  /**
   * Adds a new Command to the group with a given timeout. The Command will be
   * started after all the previously added commands.
   *
   * Once the Command is started, it will be run until it finishes or the time
   * expires, whichever is sooner.  Note that the given Command will have no
   * knowledge that it is on a timer.
   *
   * Note that any requirements the given Command has will be added to the
   * group. For this reason, a Command's requirements can not be changed after
   * being added to a group.
   *
   * It is recommended that this method be called in the constructor.
   *
   * @param command The Command to be added
   * @param timeout The timeout (in seconds)
   */
  void AddSequential(Command* command, double timeout);

  /**
   * Adds a new child Command to the group. The Command will be started after
   * all the previously added Commands.
   *
   * Instead of waiting for the child to finish, a CommandGroup will have it run
   * at the same time as the subsequent Commands. The child will run until
   * either it finishes, a new child with conflicting requirements is started,
   * or the main sequence runs a Command with conflicting requirements. In the
   * latter two cases, the child will be canceled even if it says it can't be
   * interrupted.
   *
   * Note that any requirements the given Command has will be added to the
   * group. For this reason, a Command's requirements can not be changed after
   * being added to a group.
   *
   * It is recommended that this method be called in the constructor.
   *
   * @param command The command to be added
   */
  void AddParallel(Command* command);

  /**
   * Adds a new child Command to the group with the given timeout. The Command
   * will be started after all the previously added Commands.
   *
   * Once the Command is started, it will run until it finishes, is interrupted,
   * or the time expires, whichever is sooner. Note that the given Command will
   * have no knowledge that it is on a timer.
   *
   * Instead of waiting for the child to finish, a CommandGroup will have it run
   * at the same time as the subsequent Commands. The child will run until
   * either it finishes, the timeout expires, a new child with conflicting
   * requirements is started, or the main sequence runs a Command with
   * conflicting requirements. In the latter two cases, the child will be
   * canceled even if it says it can't be interrupted.
   *
   * Note that any requirements the given Command has will be added to the
   * group. For this reason, a Command's requirements can not be changed after
   * being added to a group.
   *
   * It is recommended that this method be called in the constructor.
   *
   * @param command The command to be added
   * @param timeout The timeout (in seconds)
   */
  void AddParallel(Command* command, double timeout);

  bool IsInterruptible() const;

  int GetSize() const;

 protected:
  /**
   * Can be overridden by teams.
   */
  virtual void Initialize();

  /**
   * Can be overridden by teams.
   */
  virtual void Execute();

  /**
   * Can be overridden by teams.
   */
  virtual bool IsFinished();

  /**
   * Can be overridden by teams.
   */
  virtual void End();

  /**
   * Can be overridden by teams.
   */
  virtual void Interrupted();

  virtual void _Initialize();
  virtual void _Execute();
  virtual void _End();
  virtual void _Interrupted();

 private:
  void CancelConflicts(Command* command);

  // The commands in this group (stored in entries)
  std::vector<CommandGroupEntry> m_commands;

  // The active children in this group (stored in entries)
  std::vector<CommandGroupEntry*> m_children;

  // The current command, -1 signifies that none have been run
  int m_currentCommandIndex = -1;
};

}  // namespace frc
