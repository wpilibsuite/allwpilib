// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/SmallVector.h"

namespace wpi {

/**
 * Helper class for building Sendable dashboard representations.
 */
class SendableBuilder {
 public:
  /**
   * The backend kinds used for the sendable builder.
   */
  enum BackendKind {
    /// Unknown.
    kUnknown,
    /// NetworkTables.
    kNetworkTables
  };

  virtual ~SendableBuilder() = default;

  /**
   * Set the string representation of the named data type that will be used
   * by the smart dashboard for this sendable.
   *
   * @param type    data type
   */
  virtual void SetSmartDashboardType(std::string_view type) = 0;

  /**
   * Set a flag indicating if this sendable should be treated as an actuator.
   * By default this flag is false.
   *
   * @param value   true if actuator, false if not
   */
  virtual void SetActuator(bool value) = 0;

  /**
   * Set the function that should be called to set the Sendable into a safe
   * state.  This is called when entering and exiting Live Window mode.
   *
   * @param func    function
   */
  virtual void SetSafeState(std::function<void()> func) = 0;

  /**
   * Add a boolean property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddBooleanProperty(std::string_view key,
                                  std::function<bool()> getter,
                                  std::function<void(bool)> setter) = 0;

  /**
   * Add a constant boolean property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstBoolean(std::string_view key, bool value) = 0;

  /**
   * Add an integer property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddIntegerProperty(std::string_view key,
                                  std::function<int64_t()> getter,
                                  std::function<void(int64_t)> setter) = 0;

  /**
   * Add a constant integer property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstInteger(std::string_view key, int64_t value) = 0;

  /**
   * Add a float property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddFloatProperty(std::string_view key,
                                std::function<float()> getter,
                                std::function<void(float)> setter) = 0;

  /**
   * Add a constant float property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstFloat(std::string_view key, float value) = 0;

  /**
   * Add a double property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddDoubleProperty(std::string_view key,
                                 std::function<double()> getter,
                                 std::function<void(double)> setter) = 0;

  /**
   * Add a constant double property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstDouble(std::string_view key, double value) = 0;

  /**
   * Add a string property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddStringProperty(
      std::string_view key, std::function<std::string()> getter,
      std::function<void(std::string_view)> setter) = 0;

  /**
   * Add a constant string property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstString(std::string_view key,
                                  std::string_view value) = 0;

  /**
   * Add a boolean array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddBooleanArrayProperty(
      std::string_view key, std::function<std::vector<int>()> getter,
      std::function<void(std::span<const int>)> setter) = 0;

  /**
   * Add a constant boolean array property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstBooleanArray(std::string_view key,
                                        std::span<const int> value) = 0;

  /**
   * Add an integer array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddIntegerArrayProperty(
      std::string_view key, std::function<std::vector<int64_t>()> getter,
      std::function<void(std::span<const int64_t>)> setter) = 0;

  /**
   * Add a constant integer array property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstIntegerArray(std::string_view key,
                                        std::span<const int64_t> value) = 0;

  /**
   * Add a float array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddFloatArrayProperty(
      std::string_view key, std::function<std::vector<float>()> getter,
      std::function<void(std::span<const float>)> setter) = 0;

  /**
   * Add a constant float array property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstFloatArray(std::string_view key,
                                      std::span<const float> value) = 0;

  /**
   * Add a double array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddDoubleArrayProperty(
      std::string_view key, std::function<std::vector<double>()> getter,
      std::function<void(std::span<const double>)> setter) = 0;

  /**
   * Add a constant double array property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstDoubleArray(std::string_view key,
                                       std::span<const double> value) = 0;

  /**
   * Add a string array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddStringArrayProperty(
      std::string_view key, std::function<std::vector<std::string>()> getter,
      std::function<void(std::span<const std::string>)> setter) = 0;

  /**
   * Add a constant string array property.
   *
   * @param key     property name
   * @param value   the value
   */
  virtual void PublishConstStringArray(std::string_view key,
                                       std::span<const std::string> value) = 0;

  /**
   * Add a raw property.
   *
   * @param key         property name
   * @param typeString  type string
   * @param getter      getter function (returns current value)
   * @param setter      setter function (sets new value)
   */
  virtual void AddRawProperty(
      std::string_view key, std::string_view typeString,
      std::function<std::vector<uint8_t>()> getter,
      std::function<void(std::span<const uint8_t>)> setter) = 0;

  /**
   * Add a constant raw property.
   *
   * @param key     property name
   * @param typeString  type string
   * @param value   the value
   */
  virtual void PublishConstRaw(std::string_view key,
                               std::string_view typeString,
                               std::span<const uint8_t> value) = 0;

  /**
   * Add a string property (SmallString form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallStringProperty(
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
  virtual void AddSmallBooleanArrayProperty(
      std::string_view key,
      std::function<std::span<const int>(wpi::SmallVectorImpl<int>& buf)>
          getter,
      std::function<void(std::span<const int>)> setter) = 0;

  /**
   * Add an integer array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallIntegerArrayProperty(
      std::string_view key,
      std::function<
          std::span<const int64_t>(wpi::SmallVectorImpl<int64_t>& buf)>
          getter,
      std::function<void(std::span<const int64_t>)> setter) = 0;

  /**
   * Add a float array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallFloatArrayProperty(
      std::string_view key,
      std::function<std::span<const float>(wpi::SmallVectorImpl<float>& buf)>
          getter,
      std::function<void(std::span<const float>)> setter) = 0;

  /**
   * Add a double array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallDoubleArrayProperty(
      std::string_view key,
      std::function<std::span<const double>(wpi::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(std::span<const double>)> setter) = 0;

  /**
   * Add a string array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallStringArrayProperty(
      std::string_view key,
      std::function<
          std::span<const std::string>(wpi::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(std::span<const std::string>)> setter) = 0;

  /**
   * Add a raw property (SmallVector form).
   *
   * @param key     property name
   * @param typeString  type string
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallRawProperty(
      std::string_view key, std::string_view typeString,
      std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
          getter,
      std::function<void(std::span<const uint8_t>)> setter) = 0;

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
