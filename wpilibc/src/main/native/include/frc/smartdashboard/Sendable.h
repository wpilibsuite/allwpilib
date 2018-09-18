/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <wpi/Twine.h>

namespace frc {

class SendableBuilder;

class Sendable {
 public:
  Sendable() = default;
  virtual ~Sendable() = default;

  Sendable(Sendable&&) = default;
  Sendable& operator=(Sendable&&) = default;

  /**
   * Gets the name of this Sendable object.
   *
   * @return Name
   */
  virtual std::string GetName() const = 0;

  /**
   * Sets the name of this Sendable object.
   *
   * @param name name
   */
  virtual void SetName(const wpi::Twine& name) = 0;

  /**
   * Sets both the subsystem name and device name of this Sendable object.
   *
   * @param subsystem subsystem name
   * @param name device name
   */
  void SetName(const wpi::Twine& subsystem, const wpi::Twine& name) {
    SetSubsystem(subsystem);
    SetName(name);
  }

  /**
   * Gets the subsystem name of this Sendable object.
   *
   * @return Subsystem name
   */
  virtual std::string GetSubsystem() const = 0;

  /**
   * Sets the subsystem name of this Sendable object.
   *
   * @param subsystem subsystem name
   */
  virtual void SetSubsystem(const wpi::Twine& subsystem) = 0;

  /**
   * Initializes this Sendable object.
   *
   * @param builder sendable builder
   */
  virtual void InitSendable(SendableBuilder& builder) = 0;
};

}  // namespace frc
