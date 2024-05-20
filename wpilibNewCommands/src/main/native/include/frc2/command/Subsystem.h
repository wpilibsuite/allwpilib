// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <utility>

#include <wpi/FunctionExtras.h>

#include "frc2/command/CommandScheduler.h"

namespace frc2 {
class Command;
class CommandPtr;
/**
 * A robot subsystem.  Subsystems are the basic unit of robot organization in
 * the Command-based framework; they encapsulate low-level hardware objects
 * (motor controllers, sensors, etc) and provide methods through which they can
 * be used by Commands.  Subsystems are used by the CommandScheduler's resource
 * management system to ensure multiple robot actions are not "fighting" over
 * the same hardware; Commands that use a subsystem should include that
 * subsystem in their GetRequirements() method, and resources used within a
 * subsystem should generally remain encapsulated and not be shared by other
 * parts of the robot.
 *
 * <p>Subsystems must be registered with the scheduler with the
 * CommandScheduler.RegisterSubsystem() method in order for the
 * Periodic() method to be called.  It is recommended that this method be called
 * from the constructor of users' Subsystem implementations.  The
 * SubsystemBase class offers a simple base for user implementations
 * that handles this.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Command
 * @see CommandScheduler
 * @see SubsystemBase
 */
class Subsystem {
 public:
  virtual ~Subsystem();
  /**
   * This method is called periodically by the CommandScheduler.  Useful for
   * updating subsystem-specific state that you don't want to offload to a
   * Command.  Teams should try to be consistent within their own codebases
   * about which responsibilities will be handled by Commands, and which will be
   * handled here.
   */
  virtual void Periodic();

  /**
   * This method is called periodically by the CommandScheduler.  Useful for
   * updating subsystem-specific state that needs to be maintained for
   * simulations, such as for updating simulation classes and setting simulated
   * sensor readings.
   */
  virtual void SimulationPeriodic();

  /**
   * Gets the name of this Subsystem.
   *
   * @return Name
   */
  virtual std::string GetName() const;

  /**
   * Sets the default Command of the subsystem.  The default command will be
   * automatically scheduled when no other commands are scheduled that require
   * the subsystem. Default commands should generally not end on their own, i.e.
   * their IsFinished() method should always return false.  Will automatically
   * register this subsystem with the CommandScheduler.
   *
   * @param defaultCommand the default command to associate with this subsystem
   */
  template <std::derived_from<Command> T>
  void SetDefaultCommand(T&& defaultCommand) {
    CommandScheduler::GetInstance().SetDefaultCommand(
        this, std::forward<T>(defaultCommand));
  }

  /**
   * Sets the default Command of the subsystem.  The default command will be
   * automatically scheduled when no other commands are scheduled that require
   * the subsystem. Default commands should generally not end on their own, i.e.
   * their IsFinished() method should always return false.  Will automatically
   * register this subsystem with the CommandScheduler.
   *
   * @param defaultCommand the default command to associate with this subsystem
   */
  void SetDefaultCommand(CommandPtr&& defaultCommand);

  /**
   * Removes the default command for the subsystem.  This will not cancel the
   * default command if it is currently running.
   */
  void RemoveDefaultCommand();

  /**
   * Gets the default command for this subsystem.  Returns null if no default
   * command is currently associated with the subsystem.
   *
   * @return the default command associated with this subsystem
   */
  Command* GetDefaultCommand() const;

  /**
   * Returns the command currently running on this subsystem.  Returns null if
   * no command is currently scheduled that requires this subsystem.
   *
   * @return the scheduled command currently requiring this subsystem
   */
  Command* GetCurrentCommand() const;

  /**
   * Registers this subsystem with the CommandScheduler, allowing its
   * Periodic() method to be called when the scheduler runs.
   */
  void Register();

  /**
   * Constructs a command that runs an action once and finishes. Requires this
   * subsystem.
   *
   * @param action the action to run
   */
  [[nodiscard]]
  CommandPtr RunOnce(std::function<void()> action);

  /**
   * Constructs a command that runs an action every iteration until interrupted.
   * Requires this subsystem.
   *
   * @param action the action to run
   */
  [[nodiscard]]
  CommandPtr Run(std::function<void()> action);

  /**
   * Constructs a command that runs an action once and another action when the
   * command is interrupted. Requires this subsystem.
   *
   * @param start the action to run on start
   * @param end the action to run on interrupt
   */
  [[nodiscard]]
  CommandPtr StartEnd(std::function<void()> start, std::function<void()> end);

  /**
   * Constructs a command that runs an action every iteration until interrupted,
   * and then runs a second action. Requires this subsystem.
   *
   * @param run the action to run every iteration
   * @param end the action to run on interrupt
   */
  [[nodiscard]]
  CommandPtr RunEnd(std::function<void()> run, std::function<void()> end);

  /**
   * Constructs a command that runs an action once, and then runs an action
   * every iteration until interrupted. Requires this subsystem.
   *
   * @param start the action to run on start
   * @param run the action to run every iteration
   */
  [[nodiscard]]
  CommandPtr StartRun(std::function<void()> start, std::function<void()> run);

  /**
   * Constructs a DeferredCommand with the provided supplier. This subsystem is
   * added as a requirement.
   *
   * @param supplier the command supplier.
   * @return the command.
   */
  [[nodiscard]]
  CommandPtr Defer(wpi::unique_function<CommandPtr()> supplier);
};
}  // namespace frc2
