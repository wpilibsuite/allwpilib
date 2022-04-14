// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/deprecated.h>

namespace wpi {
class Sendable;
}  // namespace wpi

namespace frc {

/**
 * The LiveWindow class is the public interface for putting sensors and
 * actuators on the LiveWindow.
 */
class LiveWindow {
 public:
  /**
   * Get an instance of the LiveWindow main class.
   *
   * This is a singleton to guarantee that there is only a single instance
   * regardless of how many times GetInstance is called.
   * @deprecated Use the static methods unless guaranteeing LiveWindow is
   * instantiated
   */
  WPI_DEPRECATED("Use static methods")
  static LiveWindow* GetInstance();

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
