// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>

namespace frc {

class Sendable;

/**
 * The LiveWindow class is the public interface for putting sensors and
 * actuators on the LiveWindow.
 */
class LiveWindow {
 public:
  LiveWindow(const LiveWindow&) = delete;
  LiveWindow& operator=(const LiveWindow&) = delete;

  static std::function<void()> enabled;
  static std::function<void()> disabled;

  /**
   * Get an instance of the LiveWindow main class.
   *
   * This is a singleton to guarantee that there is only a single instance
   * regardless of how many times GetInstance is called.
   */
  static LiveWindow* GetInstance();

  /**
   * Enable telemetry for a single component.
   *
   * @param sendable component
   */
  static void EnableTelemetry(Sendable* component);

  /**
   * Disable telemetry for a single component.
   *
   * @param sendable component
   */
  static void DisableTelemetry(Sendable* component);

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
  LiveWindow();

  struct Impl;
  std::unique_ptr<Impl> m_impl;

  /**
   * Updates the entries, without using a mutex or lock.
   */
  static void UpdateValuesUnsafe();
};

}  // namespace frc
