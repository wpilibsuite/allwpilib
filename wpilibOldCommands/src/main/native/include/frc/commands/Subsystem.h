// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class Command;

class Subsystem : public ErrorBase,
                  public Sendable,
                  public SendableHelper<Subsystem> {
  friend class Scheduler;

 public:
  /**
   * Creates a subsystem with the given name.
   *
   * @param name the name of the subsystem
   */
  explicit Subsystem(const wpi::Twine& name);

  Subsystem(Subsystem&&) = default;
  Subsystem& operator=(Subsystem&&) = default;

  /**
   * Sets the default command. If this is not called or is called with null,
   * then there will be no default command for the subsystem.
   *
   * <b>WARNING:</b> This should <b>NOT</b> be called in a constructor if the
   * subsystem is a singleton.
   *
   * @param command the default command (or null if there should be none)
   */
  void SetDefaultCommand(Command* command);

  /**
   * Returns the default command (or null if there is none).
   *
   * @return the default command
   */
  Command* GetDefaultCommand();

  /**
   * Returns the default command name, or empty string is there is none.
   *
   * @return the default command name
   */
  wpi::StringRef GetDefaultCommandName();

  /**
   * Sets the current command.
   *
   * @param command the new current command
   */
  void SetCurrentCommand(Command* command);

  /**
   * Returns the command which currently claims this subsystem.
   *
   * @return the command which currently claims this subsystem
   */
  Command* GetCurrentCommand() const;

  /**
   * Returns the current command name, or empty string if no current command.
   *
   * @return the current command name
   */
  wpi::StringRef GetCurrentCommandName() const;

  /**
   * When the run method of the scheduler is called this method will be called.
   */
  virtual void Periodic();

  /**
   * Initialize the default command for this subsystem.
   *
   * This is meant to be the place to call SetDefaultCommand in a subsystem and
   * will be called on all the subsystems by the CommandBase method before the
   * program starts running by using the list of all registered Subsystems
   * inside the Scheduler.
   *
   * This should be overridden by a Subsystem that has a default Command
   */
  virtual void InitDefaultCommand();

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
  void SetName(const wpi::Twine& name);

  /**
   * Gets the subsystem name of this Subsystem.
   *
   * @return Subsystem name
   */
  std::string GetSubsystem() const;

  /**
   * Sets the subsystem name of this Subsystem.
   *
   * @param subsystem subsystem name
   */
  void SetSubsystem(const wpi::Twine& subsystem);

  /**
   * Associate a Sendable with this Subsystem.
   * Also update the child's name.
   *
   * @param name name to give child
   * @param child sendable
   */
  void AddChild(const wpi::Twine& name, std::shared_ptr<Sendable> child);

  /**
   * Associate a Sendable with this Subsystem.
   * Also update the child's name.
   *
   * @param name name to give child
   * @param child sendable
   */
  void AddChild(const wpi::Twine& name, Sendable* child);

  /**
   * Associate a Sendable with this Subsystem.
   * Also update the child's name.
   *
   * @param name name to give child
   * @param child sendable
   */
  void AddChild(const wpi::Twine& name, Sendable& child);

  /**
   * Associate a {@link Sendable} with this Subsystem.
   *
   * @param child sendable
   */
  void AddChild(std::shared_ptr<Sendable> child);

  /**
   * Associate a {@link Sendable} with this Subsystem.
   *
   * @param child sendable
   */
  void AddChild(Sendable* child);

  /**
   * Associate a {@link Sendable} with this Subsystem.
   *
   * @param child sendable
   */
  void AddChild(Sendable& child);

 private:
  /**
   * Call this to alert Subsystem that the current command is actually the
   * command.
   *
   * Sometimes, the Subsystem is told that it has no command while the Scheduler
   * is going through the loop, only to be soon after given a new one. This will
   * avoid that situation.
   */
  void ConfirmCommand();

  Command* m_currentCommand = nullptr;
  bool m_currentCommandChanged = true;
  Command* m_defaultCommand = nullptr;
  bool m_initializedDefaultCommand = false;

 public:
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
