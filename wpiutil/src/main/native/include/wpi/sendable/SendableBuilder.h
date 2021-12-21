// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/SmallVector.h"
#include "wpi/span.h"

namespace wpi {

class SendableBuilder {
 public:
  /**
   * The backend kinds used for the sendable builder.
   */
  enum BackendKind { kUnknown, kNetworkTables };

  virtual ~SendableBuilder() = default;

  /**
   * Set the string representation of the named data type that will be used
   * by the smart dashboard for this sendable.
   *
   * @param type    data type
   */
  virtual SendableBuilder& SetSmartDashboardType(std::string_view type) = 0;

  /**
   * Set a flag indicating if this sendable should be treated as an actuator.
   * By default this flag is false.
   *
   * @param value   true if actuator, false if not
   */
  virtual SendableBuilder& SetActuator(bool value) = 0;

  /**
   * Set the function that should be called to set the Sendable into a safe
   * state.  This is called when entering and exiting Live Window mode.
   *
   * @param func    function
   */
  virtual SendableBuilder& SetSafeState(std::function<void()> func) = 0;

  /**
   * Add a boolean property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddBooleanProperty(
      std::string_view key, std::function<bool()> getter,
      std::function<void(bool)> setter) = 0;

  /**
   * Add a double property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddDoubleProperty(
      std::string_view key, std::function<double()> getter,
      std::function<void(double)> setter) = 0;

  /**
   * Add a string property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddStringProperty(
      std::string_view key, std::function<std::string()> getter,
      std::function<void(std::string_view)> setter) = 0;

  /**
   * Add a boolean array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddBooleanArrayProperty(
      std::string_view key, std::function<std::vector<int>()> getter,
      std::function<void(wpi::span<const int>)> setter) = 0;

  /**
   * Add a double array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddDoubleArrayProperty(
      std::string_view key, std::function<std::vector<double>()> getter,
      std::function<void(wpi::span<const double>)> setter) = 0;

  /**
   * Add a string array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddStringArrayProperty(
      std::string_view key, std::function<std::vector<std::string>()> getter,
      std::function<void(wpi::span<const std::string>)> setter) = 0;

  /**
   * Add a raw property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddRawProperty(
      std::string_view key, std::function<std::string()> getter,
      std::function<void(std::string_view)> setter) = 0;

  /**
   * Add a string property (SmallString form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddSmallStringProperty(
      std::string_view key,
      std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(std::string_view)> setter) = 0;

  /**
   * Add a boolean array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddSmallBooleanArrayProperty(
      std::string_view key,
      std::function<wpi::span<const int>(wpi::SmallVectorImpl<int>& buf)>
          getter,
      std::function<void(wpi::span<const int>)> setter) = 0;

  /**
   * Add a double array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddSmallDoubleArrayProperty(
      std::string_view key,
      std::function<wpi::span<const double>(wpi::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(wpi::span<const double>)> setter) = 0;

  /**
   * Add a string array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddSmallStringArrayProperty(
      std::string_view key,
      std::function<
          wpi::span<const std::string>(wpi::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(wpi::span<const std::string>)> setter) = 0;

  /**
   * Add a raw property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual SendableBuilder& AddSmallRawProperty(
      std::string_view key,
      std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(std::string_view)> setter) = 0;

  /**
   * Gets the kind of backend being used.
   *
   * @return Backend kind
   */
  virtual BackendKind GetBackendKind() const = 0;

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  virtual bool IsPublished() const = 0;

  /**
   * Update the published values by calling the getters for all properties.
   */
  virtual void Update() = 0;

  /**
   * Clear properties.
   */
  virtual void ClearProperties() = 0;
};

}  // namespace wpi
