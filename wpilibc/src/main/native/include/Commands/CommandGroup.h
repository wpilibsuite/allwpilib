/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <list>
#include <vector>

#include <llvm/Twine.h>

#include "Commands/Command.h"
#include "Commands/CommandGroupEntry.h"

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
  explicit CommandGroup(const llvm::Twine& name);
  virtual ~CommandGroup() = default;

  void AddSequential(Command* command);
  void AddSequential(Command* command, double timeout);
  void AddParallel(Command* command);
  void AddParallel(Command* command, double timeout);
  bool IsInterruptible() const;
  int GetSize() const;

 protected:
  virtual void Initialize();
  virtual void Execute();
  virtual bool IsFinished();
  virtual void End();
  virtual void Interrupted();
  virtual void _Initialize();
  virtual void _Interrupted();
  virtual void _Execute();
  virtual void _End();

 private:
  void CancelConflicts(Command* command);

  // The commands in this group (stored in entries)
  std::vector<CommandGroupEntry> m_commands;

  // The active children in this group (stored in entries)
  std::list<CommandGroupEntry> m_children;

  // The current command, -1 signifies that none have been run
  int m_currentCommandIndex = -1;
};

}  // namespace frc
