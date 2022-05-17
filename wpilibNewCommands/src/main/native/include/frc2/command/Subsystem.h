// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc2/command/CommandScheduler.h"
#include "frc2/command/Subsystem.h"

namespace frc2 {
class Command;

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
 * Periodic() method to be called. This is done in the Subsystem constructor but
 * must be done again after calling CommandScheduler.UnregisterSubsystem().
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Command
 * @see CommandScheduler
 */
class Subsystem : public wpi::Sendable,
                      public wpi::SendableHelper<Subsystem> {
 public:
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
   * Sets the default Command of the subsystem.  The default command will be
   * automatically scheduled when no other commands are scheduled that require
   * the subsystem. Default commands should generally not end on their own, i.e.
   * their IsFinished() method should always return false.  Will automatically
   * register this subsystem with the CommandScheduler.
   *
   * @param defaultCommand the default command to associate with this subsystem
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  void SetDefaultCommand(T&& defaultCommand) {
    CommandScheduler::GetInstance().SetDefaultCommand(
        this, std::forward<T>(defaultCommand));
  }

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

  void InitSendable(wpi::SendableBuilder& builder) override;

  /**
   * Gets the name of this Subsystem.
   *
   * @return Name
   */
  std::string GetName() const;

  /**
   * Sets the name of this Subsystem.
   *
   * @param name name
   */
  void SetName(std::string_view name);

  /**
   * Gets the subsystem name of this Subsystem.
   *
   * @return Subsystem name
   */
  std::string GetSubsystem() const;

  /**
   * Sets the subsystem name of this Subsystem.
   *
   * @param name subsystem name
   */
  void SetSubsystem(std::string_view name);

  /**
   * Associate a Sendable with this Subsystem.
   * Also update the child's name.
   *
   * @param name name to give child
   * @param child sendable
   */
  void AddChild(std::string name, wpi::Sendable* child);

 protected:
  Subsystem();
};
}  // namespace frc2
