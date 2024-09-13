// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <units/current.h>
#include <units/pressure.h>
#include <units/time.h>
#include <units/voltage.h>

#include "frc/CompressorConfigType.h"
#include "frc/PneumaticsModuleType.h"

namespace frc {
class Solenoid;
class DoubleSolenoid;
class Compressor;

/**
 * Base class for pneumatics devices.
 */
class PneumaticsBase {
 public:
  virtual ~PneumaticsBase() = default;

  /**
   * Returns whether the compressor is active or not.
   *
   * @return True if the compressor is on - otherwise false.
   */
  virtual bool GetCompressor() const = 0;

  /**
   * Returns the state of the pressure switch.
   *
   * @return True if pressure switch indicates that the system is full,
   * otherwise false.
   */
  virtual bool GetPressureSwitch() const = 0;

  /**
   * Returns the current drawn by the compressor.
   *
   * @return The current drawn by the compressor.
   */
  virtual units::ampere_t GetCompressorCurrent() const = 0;

  /** Disables the compressor. */
  virtual void DisableCompressor() = 0;

  /**
   * Enables the compressor in digital mode using the digital pressure switch.
   * The compressor will turn on when the pressure switch indicates that the
   * system is not full, and will turn off when the pressure switch indicates
   * that the system is full.
   */
  virtual void EnableCompressorDigital() = 0;

  /**
   * If supported by the device, enables the compressor in analog mode. This
   * mode uses an analog pressure sensor connected to analog channel 0 to cycle
   * the compressor. The compressor will turn on when the pressure drops below
   * {@code minPressure} and will turn off when the pressure reaches {@code
   * maxPressure}. This mode is only supported by the REV PH with the REV Analog
   * Pressure Sensor connected to analog channel 0.
   *
   * On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure. The compressor will turn on
   * when the pressure drops below this value.
   * @param maxPressure The maximum pressure. The compressor will turn
   * off when the pressure reaches this value.
   */
  virtual void EnableCompressorAnalog(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) = 0;

  /**
   * If supported by the device, enables the compressor in hybrid mode. This
   * mode uses both a digital pressure switch and an analog pressure sensor
   * connected to analog channel 0 to cycle the compressor. This mode is only
   * supported by the REV PH with the REV Analog Pressure Sensor connected to
   * analog channel 0.
   *
   * The compressor will turn on when \a both:
   *
   * - The digital pressure switch indicates the system is not full AND
   * - The analog pressure sensor indicates that the pressure in the system
   * is below the specified minimum pressure.
   *
   * The compressor will turn off when \a either:
   *
   * - The digital pressure switch is disconnected or indicates that the system
   * is full OR
   * - The pressure detected by the analog sensor is greater than the specified
   * maximum pressure.
   *
   * On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure. The compressor will turn on
   * when the pressure drops below this value and the pressure switch indicates
   * that the system is not full.
   * @param maxPressure The maximum pressure. The compressor will turn
   * off when the pressure reaches this value or the pressure switch is
   * disconnected or indicates that the system is full.
   */
  virtual void EnableCompressorHybrid(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) = 0;

  /**
   * Returns the active compressor configuration.
   *
   * @return The active compressor configuration.
   */
  virtual CompressorConfigType GetCompressorConfigType() const = 0;

  /**
   * Sets solenoids on a pneumatics module.
   *
   * @param mask Bitmask indicating which solenoids to set. The LSB represents
   * solenoid 0.
   * @param values Bitmask indicating the desired states of the solenoids. The
   * LSB represents solenoid 0.
   */
  virtual void SetSolenoids(int mask, int values) = 0;

  /**
   * Gets a bitmask of solenoid values.
   *
   * @return Bitmask containing the state of the solenoids. The LSB represents
   * solenoid 0.
   */
  virtual int GetSolenoids() const = 0;

  /**
   * Get module number for this module.
   *
   * @return module number
   */
  virtual int GetModuleNumber() const = 0;

  /**
   * Get a bitmask of disabled solenoids.
   *
   * @return Bitmask indicating disabled solenoids. The LSB represents solenoid
   * 0.
   */
  virtual int GetSolenoidDisabledList() const = 0;

  /**
   * Fire a single solenoid shot.
   *
   * @param index solenoid index
   */
  virtual void FireOneShot(int index) = 0;

  /**
   * Set the duration for a single solenoid shot.
   *
   * @param index solenoid index
   * @param duration shot duration
   */
  virtual void SetOneShotDuration(int index, units::second_t duration) = 0;

  /**
   * Check if a solenoid channel is valid.
   *
   * @param channel Channel to check
   * @return True if channel exists
   */
  virtual bool CheckSolenoidChannel(int channel) const = 0;

  /**
   * Check to see if the solenoids marked in the bitmask can be reserved, and if
   * so, reserve them.
   *
   * @param mask The bitmask of solenoids to reserve. The LSB represents
   * solenoid 0.
   * @return 0 if successful; mask of solenoids that couldn't be allocated
   * otherwise
   */
  virtual int CheckAndReserveSolenoids(int mask) = 0;

  /**
   * Unreserve the solenoids marked in the bitmask.
   *
   * @param mask The bitmask of solenoids to unreserve. The LSB represents
   * solenoid 0.
   */
  virtual void UnreserveSolenoids(int mask) = 0;

  /**
   * Reserve the compressor.
   *
   * @return true if successful; false if compressor already reserved
   */
  virtual bool ReserveCompressor() = 0;

  /**
   * Unreserve the compressor.
   */
  virtual void UnreserveCompressor() = 0;

  /**
   * If supported by the device, returns the raw voltage of the specified analog
   * input channel.
   *
   * This function is only supported by the REV PH. On CTRE PCM, this will
   * return 0.
   *
   * @param channel The analog input channel to read voltage from.
   * @return The voltage of the specified analog input channel.
   */
  virtual units::volt_t GetAnalogVoltage(int channel) const = 0;

  /**
   * If supported by the device, returns the pressure read by an analog
   * pressure sensor on the specified analog input channel.
   *
   * This function is only supported by the REV PH. On CTRE PCM, this will
   * return 0.
   *
   * @param channel The analog input channel to read pressure from.
   * @return The pressure read by an analog pressure sensor on the
   * specified analog input channel.
   */
  virtual units::pounds_per_square_inch_t GetPressure(int channel) const = 0;

  /**
   * Create a solenoid object for the specified channel.
   *
   * @param channel solenoid channel
   * @return Solenoid object
   */
  virtual Solenoid MakeSolenoid(int channel) = 0;

  /**
   * Create a double solenoid object for the specified channels.
   *
   * @param forwardChannel solenoid channel for forward
   * @param reverseChannel solenoid channel for reverse
   * @return DoubleSolenoid object
   */
  virtual DoubleSolenoid MakeDoubleSolenoid(int forwardChannel,
                                            int reverseChannel) = 0;

  /**
   * Create a compressor object.
   *
   * @return Compressor object
   */
  virtual Compressor MakeCompressor() = 0;

  /**
   * For internal use to get a module for a specific type.
   *
   * @param module module number
   * @param moduleType module type
   * @return module
   */
  static std::shared_ptr<PneumaticsBase> GetForType(
      int module, PneumaticsModuleType moduleType);

  /**
   * For internal use to get the default for a specific type.
   *
   * @param moduleType module type
   * @return module default
   */
  static int GetDefaultForType(PneumaticsModuleType moduleType);
};
}  // namespace frc
