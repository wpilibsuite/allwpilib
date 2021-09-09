// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "wpi/deprecated.h"
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
class SendableHelper {
 public:
  SendableHelper(const SendableHelper& rhs) = default;
  SendableHelper& operator=(const SendableHelper& rhs) = default;

#if !defined(_MSC_VER) && (defined(__llvm__) || __GNUC__ > 7)
  // See https://bugzilla.mozilla.org/show_bug.cgi?id=1442819
  __attribute__((no_sanitize("vptr")))
#endif
  SendableHelper(SendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    SendableRegistry::Move(static_cast<Derived*>(this),
                           static_cast<Derived*>(&rhs));
  }

#if !defined(_MSC_VER) && (defined(__llvm__) || __GNUC__ > 7)
  // See https://bugzilla.mozilla.org/show_bug.cgi?id=1442819
  __attribute__((no_sanitize("vptr")))
#endif
  SendableHelper&
  operator=(SendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    SendableRegistry::Move(static_cast<Derived*>(this),
                           static_cast<Derived*>(&rhs));
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
  std::string GetName() const {
    return SendableRegistry::GetName(static_cast<const Derived*>(this));
  }

  /**
   * Sets the name of this Sendable object.
   *
   * @deprecated use SendableRegistry::SetName()
   *
   * @param name name
   */
  WPI_DEPRECATED("use SendableRegistry::SetName()")
  void SetName(std::string_view name) {
    SendableRegistry::SetName(static_cast<Derived*>(this), name);
  }

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
    SendableRegistry::SetName(static_cast<Derived*>(this), subsystem, name);
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
    return SendableRegistry::GetSubsystem(static_cast<const Derived*>(this));
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
    SendableRegistry::SetSubsystem(static_cast<Derived*>(this), subsystem);
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
    SendableRegistry::AddChild(static_cast<Derived*>(this), child.get());
  }

  /**
   * Add a child component.
   *
   * @deprecated use SendableRegistry::AddChild()
   *
   * @param child child component
   */
  WPI_DEPRECATED("use SendableRegistry::AddChild()")
  void AddChild(void* child) {
    SendableRegistry::AddChild(static_cast<Derived*>(this), child);
  }

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
    SendableRegistry::SetName(static_cast<Derived*>(this), moduleType, channel);
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
    SendableRegistry::SetName(static_cast<Derived*>(this), moduleType,
                              moduleNumber, channel);
  }

 protected:
  SendableHelper() = default;

  ~SendableHelper() {
    // it is safe to call Remove() multiple times with the same object
    SendableRegistry::Remove(static_cast<Derived*>(this));
  }
};

}  // namespace wpi
