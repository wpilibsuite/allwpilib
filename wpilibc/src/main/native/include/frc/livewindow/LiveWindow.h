/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  std::function<void()> enabled;
  std::function<void()> disabled;

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
  void EnableTelemetry(Sendable* component);

  /**
   * Disable telemetry for a single component.
   *
   * @param sendable component
   */
  void DisableTelemetry(Sendable* component);

  /**
   * Disable ALL telemetry.
   */
  void DisableAllTelemetry();

  bool IsEnabled() const;

  /**
   * Change the enabled status of LiveWindow.
   *
   * If it changes to enabled, start livewindow running otherwise stop it
   */
  void SetEnabled(bool enabled);

  /**
   * Tell all the sensors to update (send) their values.
   *
   * Actuators are handled through callbacks on their value changing from the
   * SmartDashboard widgets.
   */
  void UpdateValues();

 private:
  LiveWindow();

  struct Impl;
  std::unique_ptr<Impl> m_impl;

  /**
   * Updates the entries, without using a mutex or lock.
   */
  void UpdateValuesUnsafe();
};

}  // namespace frc
