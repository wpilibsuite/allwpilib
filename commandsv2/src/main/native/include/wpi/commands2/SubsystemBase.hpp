// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

#include "wpi/commands2/Subsystem.hpp"

namespace frc2 {
/**
 * A base for subsystems that handles registration in the constructor, and
 * provides a more intuitive method for setting the default command.
 *
 * This class is provided by the NewCommands VendorDep
 */
class SubsystemBase : public Subsystem,
                      public wpi::Sendable,
                      public wpi::SendableHelper<SubsystemBase> {
 public:
  void InitSendable(wpi::SendableBuilder& builder) override;

  /**
   * Gets the name of this Subsystem.
   *
   * @return Name
   */
  std::string GetName() const override;

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
  /**
   * Constructor.  Telemetry/log name defaults to the classname.
   */
  SubsystemBase();
  /**
   * Constructor.
   *
   * @param name Name of the subsystem for telemetry and logging.
   */
  explicit SubsystemBase(std::string_view name);
};
}  // namespace frc2
