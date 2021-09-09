// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "wpi/deprecated.h"
#include "wpi/sendable/Sendable.h"
#include "wpi/sendable/SendableRegistry.h"

namespace wpi {

/**
 * A helper class for use with objects that add themselves to SendableRegistry.
 * It takes care of properly calling Move() and Remove() on move and
 * destruction.  No action is taken if the object is copied.
 * Use public inheritance with CRTP when using this class.
 * @tparam CRTP derived class
 */
template <typename Derived>
class SendableHelper : public Sendable {
 public:
  SendableHelper(const SendableHelper& rhs) = default;
  SendableHelper& operator=(const SendableHelper& rhs) = default;

  SendableHelper(SendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    SendableRegistry::Move(this, &rhs);
  }

  SendableHelper& operator=(SendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    SendableRegistry::Move(this, &rhs);
    return *this;
  }

  /**
   * Gets the name of this Sendable object.
   *
   * @deprecated use SendableRegistry::GetName()
   *
   * @return Name
   */
  WPI_DEPRECATED("use SendableRegistry::GetName()")
  std::string GetName() const { return SendableRegistry::GetName(this); }

  /**
   * Sets the name of this Sendable object.
   *
   * @deprecated use SendableRegistry::SetName()
   *
   * @param name name
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(std::string_view name) { SendableRegistry::SetName(this, name); }

  /**
   * Sets both the subsystem name and device name of this Sendable object.
   *
   * @deprecated use SendableRegistry::SetName()
   *
   * @param subsystem subsystem name
   * @param name device name
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(std::string_view subsystem, std::string_view name) {
    SendableRegistry::SetName(this, subsystem, name);
  }

  /**
   * Gets the subsystem name of this Sendable object.
   *
   * @deprecated use SendableRegistry::GetSubsystem().
   *
   * @return Subsystem name
   */
  WPI_DEPRECATED("use SendableRegistry::GetSubsystem()")
  std::string GetSubsystem() const {
    return SendableRegistry::GetSubsystem(this);
  }

  /**
   * Sets the subsystem name of this Sendable object.
   *
   * @deprecated use SendableRegistry::SetSubsystem()
   *
   * @param subsystem subsystem name
   */
  WPI_DEPRECATED("use SendableRegistry::SetSubsystem()")
  void SetSubsystem(std::string_view subsystem) {
    SendableRegistry::SetSubsystem(this, subsystem);
  }

 protected:
  /**
   * Add a child component.
   *
   * @deprecated use SendableRegistry::AddChild()
   *
   * @param child child component
   */
  WPI_DEPRECATED("use SendableRegistry::AddChild()")
  void AddChild(std::shared_ptr<Sendable> child) {
    SendableRegistry::AddChild(this, child.get());
  }

  /**
   * Add a child component.
   *
   * @deprecated use SendableRegistry::AddChild()
   *
   * @param child child component
   */
  WPI_DEPRECATED("use SendableRegistry::AddChild()")
  void AddChild(void* child) { SendableRegistry::AddChild(this, child); }

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @deprecated use SendableRegistry::SetName()
   *
   * @param moduleType A string that defines the module name in the label for
   *                   the value
   * @param channel    The channel number the device is plugged into
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(std::string_view moduleType, int channel) {
    SendableRegistry::SetName(this, moduleType, channel);
  }

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @deprecated use SendableRegistry::SetName()
   *
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually
   * PWM)
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(std::string_view moduleType, int moduleNumber, int channel) {
    SendableRegistry::SetName(this, moduleType, moduleNumber, channel);
  }

 protected:
  SendableHelper() = default;

  ~SendableHelper() override {
    // it is safe to call Remove() multiple times with the same object
    SendableRegistry::Remove(this);
  }
};

}  // namespace wpi
