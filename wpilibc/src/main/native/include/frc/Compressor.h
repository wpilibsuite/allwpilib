// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/CompressorConfigType.h"
#include "frc/PneumaticsBase.h"
#include "frc/PneumaticsModuleType.h"
#include "frc/SensorUtil.h"

namespace frc {

/**
 * Class for operating a compressor connected to a pneumatics module.
 *
 * The module will automatically run in closed loop mode by default whenever a
 * Solenoid object is created. For most cases, a Compressor object does not need
 * to be instantiated or used in a robot program. This class is only required in
 * cases where the robot program needs a more detailed status of the compressor
 * or to enable/disable closed loop control.
 *
 * Note: you cannot operate the compressor directly from this class as doing so
 * would circumvent the safety provided by using the pressure switch and closed
 * loop control. You can only turn off closed loop control, thereby stopping
 * the compressor from operating.
 */
class Compressor : public wpi::Sendable,
                   public wpi::SendableHelper<Compressor> {
 public:
  /**
   * Constructs a compressor for a specified module and type.
   *
   * @param module The module ID to use.
   * @param moduleType The module type to use.
   */
  Compressor(int module, PneumaticsModuleType moduleType);

  /**
   * Constructs a compressor for a default module and specified type.
   *
   * @param moduleType The module type to use.
   */
  explicit Compressor(PneumaticsModuleType moduleType);

  ~Compressor() override;

  Compressor(const Compressor&) = delete;
  Compressor& operator=(const Compressor&) = delete;

  Compressor(Compressor&&) = default;
  Compressor& operator=(Compressor&&) = default;

  /**
   * Returns whether the compressor is active or not.
   *
   * @return true if the compressor is on - otherwise false.
   */
  bool IsEnabled() const;

  /**
   * Returns the state of the pressure switch.
   *
   * @return True if pressure switch indicates that the system is not full,
   * otherwise false.
   */
  bool GetPressureSwitchValue() const;

  /**
   * Get the current drawn by the compressor.
   *
   * @return Current drawn by the compressor.
   */
  units::ampere_t GetCurrent() const;

  /**
   * If supported by the device, returns the analog input voltage (on channel
   * 0).
   *
   * This function is only supported by the REV PH. On CTRE PCM, this will
   * return 0.
   *
   * @return The analog input voltage, in volts.
   */
  units::volt_t GetAnalogVoltage() const;

  /**
   * If supported by the device, returns the pressure read by the analog
   * pressure sensor (on channel 0).
   *
   * This function is only supported by the REV PH with the REV Analog Pressure
   * Sensor. On CTRE PCM, this will return 0.
   *
   * @return The pressure read by the analog pressure sensor.
   */
  units::pounds_per_square_inch_t GetPressure() const;

  /**
   * Disable the compressor.
   */
  void Disable();

  /**
   * Enables the compressor in digital mode using the digital pressure switch.
   * The compressor will turn on when the pressure switch indicates that the
   * system is not full, and will turn off when the pressure switch indicates
   * that the system is full.
   */
  void EnableDigital();

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
   * @param minPressure The minimum pressure. The compressor will turn on when
   * the pressure drops below this value.
   * @param maxPressure The maximum pressure. The compressor will turn off when
   * the pressure reaches this value.
   */
  void EnableAnalog(units::pounds_per_square_inch_t minPressure,
                    units::pounds_per_square_inch_t maxPressure);

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
  void EnableHybrid(units::pounds_per_square_inch_t minPressure,
                    units::pounds_per_square_inch_t maxPressure);

  /**
   * Returns the active compressor configuration.
   *
   * @return The active compressor configuration.
   */
  CompressorConfigType GetConfigType() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<PneumaticsBase> m_module;
};

}  // namespace frc
