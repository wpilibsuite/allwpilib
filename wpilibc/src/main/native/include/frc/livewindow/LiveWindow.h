// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string>
#include <string_view>
namespace wpi {
class Sendable;
}  // namespace wpi

namespace frc {

/**
 * The LiveWindow class is the public interface for putting sensors and
 * actuators on the LiveWindow.
 */
class LiveWindow final {
 public:
   using UID = size_t;
  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable Object to add
   * @param name Component name
   */
  static void AddLW(wpi::Sendable* sendable, std::string_view name);

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable     Object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param channel      The channel number the device is plugged into
   */
  static void AddLW(wpi::Sendable* sendable, std::string_view moduleType,
                    int channel);

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable     Object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  static void AddLW(wpi::Sendable* sendable, std::string_view moduleType,
                    int moduleNumber, int channel);

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable Object to add
   * @param subsystem Subsystem name
   * @param name Component name
   */
  static void AddLW(wpi::Sendable* sendable, std::string_view subsystem,
                    std::string_view name);

  /**
   * Adds a child object to an object.  Adds the child object to LiveWindow
   * if it's not already present.
   *
   * @param parent Parent object
   * @param child Child object
   */
  static void AddChild(wpi::Sendable* parent, wpi::Sendable* child);

  /**
   * Adds a child object to an object.  Adds the child object to LiveWindow
   * if it's not already present.
   *
   * @param parent Parent object
   * @param child Child object
   */
  static void AddChild(wpi::Sendable* parent, void* child);

  /**
   * Removes an object from LiveWindow.
   *
   * @param sendable Object to remove
   * @return True if the object was removed; false if it was not present
   */
  static bool Remove(wpi::Sendable* sendable);

  /**
   * Moves an object in LiveWindow (for use in move constructors/assignments).
   *
   * @param to New object
   * @param from Old object
   */
  static void Move(wpi::Sendable* to, wpi::Sendable* from);

  /**
   * Determines if an object is in LiveWindow.
   *
   * @param sendable object to check
   * @return True if in LiveWindow, false if not.
   */
  static bool Contains(const wpi::Sendable* sendable);

  /**
   * Gets the name of an object.
   *
   * @param sendable Object
   * @return Name (empty if object is not in LiveWindow)
   */
  static std::string GetName(const wpi::Sendable* sendable);

  /**
   * Sets the name of an object.
   *
   * @param sendable Object
   * @param name Name
   */
  static void SetName(wpi::Sendable* sendable, std::string_view name);

  /**
   * Sets the name of an object with a channel number.
   *
   * @param sendable   Object
   * @param moduleType A string that defines the module name in the label for
   *                   the value
   * @param channel    The channel number the device is plugged into
   */
  static void SetName(wpi::Sendable* sendable, std::string_view moduleType,
                      int channel);

  /**
   * Sets the name of an object with a module and channel number.
   *
   * @param sendable     Object
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  static void SetName(wpi::Sendable* sendable, std::string_view moduleType,
                      int moduleNumber, int channel);

  /**
   * Sets both the subsystem name and device name of an object.
   *
   * @param sendable Object
   * @param subsystem Subsystem name
   * @param name Device name
   */
  static void SetName(wpi::Sendable* sendable, std::string_view subsystem,
                      std::string_view name);

  /**
   * Gets the subsystem name of an object.
   *
   * @param sendable Object
   * @return Subsystem name (empty if object is not in LiveWindow)
   */
  static std::string GetSubsystem(const wpi::Sendable* sendable);

  /**
   * Sets the subsystem name of an object.
   *
   * @param sendable Object
   * @param subsystem Subsystem name
   */
  static void SetSubsystem(wpi::Sendable* sendable, std::string_view subsystem);

  /**
   * Set function to be called when LiveWindow is enabled.
   *
   * @param func function (or nullptr for none)
   */
  static void SetEnabledCallback(std::function<void()> func);

  /**
   * Set function to be called when LiveWindow is disabled.
   *
   * @param func function (or nullptr for none)
   */
  static void SetDisabledCallback(std::function<void()> func);

  /**
   * Enable telemetry for a single component.
   *
   * @param component sendable
   */
  static void EnableTelemetry(wpi::Sendable* component);

  /**
   * Disable telemetry for a single component.
   *
   * @param component sendable
   */
  static void DisableTelemetry(wpi::Sendable* component);

  /**
   * Disable ALL telemetry.
   */
  static void DisableAllTelemetry();

  /**
   * Enable ALL telemetry.
   */
  static void EnableAllTelemetry();

  static bool IsEnabled();

  /**
   * Change the enabled status of LiveWindow.
   *
   * If it changes to enabled, start livewindow running otherwise stop it
   */
  static void SetEnabled(bool enabled);

  /**
   * Tell all the sensors to update (send) their values.
   *
   * Actuators are handled through callbacks on their value changing from the
   * SmartDashboard widgets.
   */
  static void UpdateValues();

 private:
  LiveWindow() = default;

  /**
   * Updates the entries, without using a mutex or lock.
   */
  static void UpdateValuesUnsafe();
};

}  // namespace frc
