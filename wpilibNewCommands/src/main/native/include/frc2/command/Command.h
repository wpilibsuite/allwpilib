/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>

#include <frc/ErrorBase.h>
#include <units/time.h>
#include <wpi/ArrayRef.h>
#include <wpi/Demangle.h>
#include <wpi/SmallSet.h>

#include "frc2/command/Subsystem.h"

namespace frc2 {

template <typename T>
std::string GetTypeName(const T& type) {
  return wpi::Demangle(typeid(type).name());
}

class ParallelCommandGroup;
class ParallelRaceGroup;
class ParallelDeadlineGroup;
class SequentialCommandGroup;
class PerpetualCommand;
class ProxyScheduleCommand;

/**
 * A state machine representing a complete action to be performed by the robot.
 * Commands are run by the CommandScheduler, and can be composed into
 * CommandGroups to allow users to build complicated multi-step actions without
 * the need to roll the state machine logic themselves.
 *
 * <p>Commands are run synchronously from the main robot loop; no multithreading
 * is used, unless specified explicitly from the command implementation.
 *
 * <p>Note: ALWAYS create a subclass by extending CommandHelper<Base, Subclass>,
 * or decorators will not function!
 *
 * @see CommandScheduler
 * @see CommandHelper
 */
class Command : public frc::ErrorBase {
 public:
  Command() = default;
  virtual ~Command();

  Command(const Command&);
  Command& operator=(const Command&);
  Command(Command&&) = default;
  Command& operator=(Command&&) = default;

  /**
   * The initial subroutine of a command.  Called once when the command is
   * initially scheduled.
   */
  virtual void Initialize();

  /**
   * The main body of a command.  Called repeatedly while the command is
   * scheduled.
   */
  virtual void Execute();

  /**
   * The action to take when the command ends.  Called when either the command
   * finishes normally, or when it interrupted/canceled.
   *
   * @param interrupted whether the command was interrupted/canceled
   */
  virtual void End(bool interrupted);

  /**
   * Whether the command has finished.  Once a command finishes, the scheduler
   * will call its end() method and un-schedule it.
   *
   * @return whether the command has finished.
   */
  virtual bool IsFinished() { return false; }

  /**
   * Specifies the set of subsystems used by this command.  Two commands cannot
   * use the same subsystem at the same time.  If the command is scheduled as
   * interruptible and another command is scheduled that shares a requirement,
   * the command will be interrupted.  Else, the command will not be scheduled.
   * If no subsystems are required, return an empty set.
   *
   * <p>Note: it is recommended that user implementations contain the
   * requirements as a field, and return that field here, rather than allocating
   * a new set every time this is called.
   *
   * @return the set of subsystems that are required
   */
  virtual wpi::SmallSet<Subsystem*, 4> GetRequirements() const = 0;

  /**
   * Decorates this command with a timeout.  If the specified timeout is
   * exceeded before the command finishes normally, the command will be
   * interrupted and un-scheduled.  Note that the timeout only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param duration the timeout duration
   * @return the command with the timeout added
   */
  ParallelRaceGroup WithTimeout(units::second_t duration) &&;

  /**
   * Decorates this command with an interrupt condition.  If the specified
   * condition becomes true before the command finishes normally, the command
   * will be interrupted and un-scheduled. Note that this only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  ParallelRaceGroup WithInterrupt(std::function<bool()> condition) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  SequentialCommandGroup BeforeStarting(
      std::function<void()> toRun,
      std::initializer_list<Subsystem*> requirements) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  SequentialCommandGroup BeforeStarting(
      std::function<void()> toRun,
      wpi::ArrayRef<Subsystem*> requirements = {}) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  SequentialCommandGroup AndThen(
      std::function<void()> toRun,
      std::initializer_list<Subsystem*> requirements) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  SequentialCommandGroup AndThen(
      std::function<void()> toRun,
      wpi::ArrayRef<Subsystem*> requirements = {}) &&;

  /**
   * Decorates this command to run perpetually, ignoring its ordinary end
   * conditions.  The decorated command can still be interrupted or canceled.
   *
   * @return the decorated command
   */
  PerpetualCommand Perpetually() &&;

  /**
   * Decorates this command to run "by proxy" by wrapping it in a {@link
   * ProxyScheduleCommand}. This is useful for "forking off" from command groups
   * when the user does not wish to extend the command's requirements to the
   * entire command group.
   *
   * @return the decorated command
   */
  ProxyScheduleCommand AsProxy();

  /**
   * Schedules this command.
   *
   * @param interruptible whether this command can be interrupted by another
   * command that shares one of its requirements
   */
  void Schedule(bool interruptible);

  /**
   * Schedules this command, defaulting to interruptible.
   */
  void Schedule() { Schedule(true); }

  /**
   * Cancels this command.  Will call the command's interrupted() method.
   * Commands will be canceled even if they are not marked as interruptible.
   */
  void Cancel();

  /**
   * Whether or not the command is currently scheduled.  Note that this does not
   * detect whether the command is being run by a CommandGroup, only whether it
   * is directly being run by the scheduler.
   *
   * @return Whether the command is scheduled.
   */
  bool IsScheduled() const;

  /**
   * Whether the command requires a given subsystem.  Named "hasRequirement"
   * rather than "requires" to avoid confusion with
   * {@link
   * edu.wpi.first.wpilibj.command.Command#requires(edu.wpi.first.wpilibj.command.Subsystem)}
   *  - this may be able to be changed in a few years.
   *
   * @param requirement the subsystem to inquire about
   * @return whether the subsystem is required
   */
  bool HasRequirement(Subsystem* requirement) const;

  /**
   * Whether the command is currently grouped in a command group.  Used as extra
   * insurance to prevent accidental independent use of grouped commands.
   */
  bool IsGrouped() const;

  /**
   * Sets whether the command is currently grouped in a command group.  Can be
   * used to "reclaim" a command if a group is no longer going to use it.  NOT
   * ADVISED!
   */
  void SetGrouped(bool grouped);

  /**
   * Whether the given command should run when the robot is disabled.  Override
   * to return true if the command should run when disabled.
   *
   * @return whether the command should run when the robot is disabled
   */
  virtual bool RunsWhenDisabled() const { return false; }

  virtual std::string GetName() const;

 protected:
  /**
   * Transfers ownership of this command to a unique pointer.  Used for
   * decorator methods.
   */
  virtual std::unique_ptr<Command> TransferOwnership() && = 0;

  bool m_isGrouped = false;
};

/**
 * Checks if two commands have disjoint requirement sets.
 *
 * @param first The first command to check.
 * @param second The second command to check.
 * @return False if first and second share a requirement.
 */
bool RequirementsDisjoint(Command* first, Command* second);
}  // namespace frc2
