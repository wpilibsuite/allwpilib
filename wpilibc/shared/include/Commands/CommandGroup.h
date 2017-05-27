/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <list>
#include <string>
#include <vector>

#include "Commands/Command.h"
#include "Commands/CommandGroupEntry.h"

namespace frc {

/**
 * A {@link CommandGroup} is a list of commands which are executed in sequence.
 *
 * <p>Commands in a {@link CommandGroup} are added using the {@link
 * CommandGroup#AddSequential(Command) AddSequential(...)} method and are
 * called sequentially. {@link CommandGroup CommandGroups} are themselves
 * {@link Command Commands} and can be given to other
 * {@link CommandGroup CommandGroups}.</p>
 *
 * <p>{@link CommandGroup CommandGroups} will carry all of the requirements of
 * their {@link Command subcommands}.  Additional requirements can be specified
 * by calling {@link CommandGroup#Requires(Subsystem) Requires(...)} normally
 * in the constructor.</P>
 *
 * <p>CommandGroups can also execute commands in parallel, simply by adding them
 * using {@link CommandGroup#AddParallel(Command) AddParallel(...)}.</p>
 *
 * @see Command
 * @see Subsystem
 */
class CommandGroup : public Command {
 public:
  CommandGroup() = default;
  explicit CommandGroup(const std::string& name);
  virtual ~CommandGroup() = default;

  template <class... T>
  void AddParallelGroup(Command* command1, T... commands);
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

  /** The commands in this group (stored in entries) */
  std::vector<CommandGroupEntry> m_commands;

  /** The active children in this group (stored in entries) */
  std::list<CommandGroupEntry> m_children;

  /** The current command, -1 signifies that none have been run */
  int m_currentCommandIndex = -1;

  template <class... Commands>
  void _addParallelGroup(Command*, Command*, Commands...);
  void _addParallelGroup(Command*);
  template <class... Commands>
  bool _checkNull(Command*, Commands...);
  bool _checkNull(Command*);
};

template <class... Commands>
void CommandGroup::AddParallelGroup(Command* command1, Commands... commands) {
  if (!AssertUnlocked("Cannot add new command to command group")) return;
  if (_checkNull(command1) || _checkNull(commands...)) return;
  _addParallelGroup(command1, commands...);
}

template <class... Commands>
void CommandGroup::_addParallelGroup(Command* command1, Command* command2,
                                     Commands... commands) {
  AddParallel(command1);
  _asip(command2, commands...);
}

template <class... Commands>
bool CommandGroup::_checkNull(Command* command1, Commands... commands) {
  return _checkNull(command1) || _checkNull(commands...);
}

}  // namespace frc
