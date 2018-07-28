/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/ArrayRef.h>
#include <wpi/SmallVector.h>
#include <wpi/Twine.h>

namespace frc {

class SendableBuilder {
 public:
  virtual ~SendableBuilder() = default;

  /**
   * Set the string representation of the named data type that will be used
   * by the smart dashboard for this sendable.
   *
   * @param type    data type
   */
  virtual void SetSmartDashboardType(const wpi::Twine& type) = 0;

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
   * Set the function that should be called to update the network table
   * for things other than properties.  Note this function is not passed
   * the network table object; instead it should use the entry handles
   * returned by GetEntry().
   *
   * @param func    function
   */
  virtual void SetUpdateTable(std::function<void()> func) = 0;

  /**
   * Add a property without getters or setters.  This can be used to get
   * entry handles for the function called by SetUpdateTable().
   *
   * @param key   property name
   * @return Network table entry
   */
  virtual nt::NetworkTableEntry GetEntry(const wpi::Twine& key) = 0;

  /**
   * Add a boolean property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddBooleanProperty(const wpi::Twine& key,
                                  std::function<bool()> getter,
                                  std::function<void(bool)> setter) = 0;

  /**
   * Add a double property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddDoubleProperty(const wpi::Twine& key,
                                 std::function<double()> getter,
                                 std::function<void(double)> setter) = 0;

  /**
   * Add a string property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddStringProperty(
      const wpi::Twine& key, std::function<std::string()> getter,
      std::function<void(wpi::StringRef)> setter) = 0;

  /**
   * Add a boolean array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddBooleanArrayProperty(
      const wpi::Twine& key, std::function<std::vector<int>()> getter,
      std::function<void(wpi::ArrayRef<int>)> setter) = 0;

  /**
   * Add a double array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddDoubleArrayProperty(
      const wpi::Twine& key, std::function<std::vector<double>()> getter,
      std::function<void(wpi::ArrayRef<double>)> setter) = 0;

  /**
   * Add a string array property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddStringArrayProperty(
      const wpi::Twine& key, std::function<std::vector<std::string>()> getter,
      std::function<void(wpi::ArrayRef<std::string>)> setter) = 0;

  /**
   * Add a raw property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddRawProperty(const wpi::Twine& key,
                              std::function<std::string()> getter,
                              std::function<void(wpi::StringRef)> setter) = 0;

  /**
   * Add a NetworkTableValue property.
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddValueProperty(
      const wpi::Twine& key, std::function<std::shared_ptr<nt::Value>()> getter,
      std::function<void(std::shared_ptr<nt::Value>)> setter) = 0;

  /**
   * Add a string property (SmallString form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallStringProperty(
      const wpi::Twine& key,
      std::function<wpi::StringRef(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(wpi::StringRef)> setter) = 0;

  /**
   * Add a boolean array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallBooleanArrayProperty(
      const wpi::Twine& key,
      std::function<wpi::ArrayRef<int>(wpi::SmallVectorImpl<int>& buf)> getter,
      std::function<void(wpi::ArrayRef<int>)> setter) = 0;

  /**
   * Add a double array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallDoubleArrayProperty(
      const wpi::Twine& key,
      std::function<wpi::ArrayRef<double>(wpi::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(wpi::ArrayRef<double>)> setter) = 0;

  /**
   * Add a string array property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallStringArrayProperty(
      const wpi::Twine& key,
      std::function<
          wpi::ArrayRef<std::string>(wpi::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(wpi::ArrayRef<std::string>)> setter) = 0;

  /**
   * Add a raw property (SmallVector form).
   *
   * @param key     property name
   * @param getter  getter function (returns current value)
   * @param setter  setter function (sets new value)
   */
  virtual void AddSmallRawProperty(
      const wpi::Twine& key,
      std::function<wpi::StringRef(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(wpi::StringRef)> setter) = 0;
};

}  // namespace frc
