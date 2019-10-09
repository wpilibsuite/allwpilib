/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/Twine.h>
#include <wpi/deprecated.h>

#include "frc/smartdashboard/SendableRegistry.h"

namespace frc {

/**
 * A helper class for use with objects that add themselves to SendableRegistry.
 * It takes care of properly calling Move() and Remove() on move and
 * destruction.  No action is taken if the object is copied.
 * Use public inheritance with CRTP when using this class.
 * @tparam CRTP derived class
 */
template <typename Derived>
class SendableHelper {
 public:
  SendableHelper(const SendableHelper& rhs) = default;
  SendableHelper& operator=(const SendableHelper& rhs) = default;

  SendableHelper(SendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    SendableRegistry::GetInstance().Move(static_cast<Derived*>(this),
                                         static_cast<Derived*>(&rhs));
  }

  SendableHelper& operator=(SendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    SendableRegistry::GetInstance().Move(static_cast<Derived*>(this),
                                         static_cast<Derived*>(&rhs));
    return *this;
  }

  /**
   * Gets the name of this Sendable object.
   *
   * @return Name
   */
  WPI_DEPRECATED("use SendableRegistry::GetName()")
  std::string GetName() const {
    return SendableRegistry::GetInstance().GetName(
        static_cast<const Derived*>(this));
  }

  /**
   * Sets the name of this Sendable object.
   *
   * @param name name
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(const wpi::Twine& name) {
    SendableRegistry::GetInstance().SetName(static_cast<Derived*>(this), name);
  }

  /**
   * Sets both the subsystem name and device name of this Sendable object.
   *
   * @param subsystem subsystem name
   * @param name device name
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(const wpi::Twine& subsystem, const wpi::Twine& name) {
    SendableRegistry::GetInstance().SetName(static_cast<Derived*>(this),
                                            subsystem, name);
  }

  /**
   * Gets the subsystem name of this Sendable object.
   *
   * @return Subsystem name
   */
  WPI_DEPRECATED("use SendableRegistry::GetSubsystem()")
  std::string GetSubsystem() const {
    return SendableRegistry::GetInstance().GetSubsystem(
        static_cast<const Derived*>(this));
  }

  /**
   * Sets the subsystem name of this Sendable object.
   *
   * @param subsystem subsystem name
   */
  WPI_DEPRECATED("use SendableRegistry::SetSubsystem()")
  void SetSubsystem(const wpi::Twine& subsystem) {
    SendableRegistry::GetInstance().SetSubsystem(static_cast<Derived*>(this),
                                                 subsystem);
  }

 protected:
  /**
   * Add a child component.
   *
   * @param child child component
   */
  WPI_DEPRECATED("use SendableRegistry::AddChild()")
  void AddChild(std::shared_ptr<Sendable> child) {
    SendableRegistry::GetInstance().AddChild(static_cast<Derived*>(this),
                                             child.get());
  }

  /**
   * Add a child component.
   *
   * @param child child component
   */
  WPI_DEPRECATED("use SendableRegistry::AddChild()")
  void AddChild(void* child) {
    SendableRegistry::GetInstance().AddChild(static_cast<Derived*>(this),
                                             child);
  }

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @param moduleType A string that defines the module name in the label for
   *                   the value
   * @param channel    The channel number the device is plugged into
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(const wpi::Twine& moduleType, int channel) {
    SendableRegistry::GetInstance().SetName(static_cast<Derived*>(this),
                                            moduleType, channel);
  }

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually
   * PWM)
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(const wpi::Twine& moduleType, int moduleNumber, int channel) {
    SendableRegistry::GetInstance().SetName(static_cast<Derived*>(this),
                                            moduleType, moduleNumber, channel);
  }

 protected:
  SendableHelper() = default;

  ~SendableHelper() {
    // it is safe to call Remove() multiple times with the same object
    SendableRegistry::GetInstance().Remove(static_cast<Derived*>(this));
  }
};

}  // namespace frc
