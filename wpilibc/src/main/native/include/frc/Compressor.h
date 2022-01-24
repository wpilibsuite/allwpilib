// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/deprecated.h>
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
   * Starts closed-loop control. Note that closed loop control is enabled by
   * default.
   *
   * @deprecated Use EnableDigital() instead.
   */
  WPI_DEPRECATED("Use EnableDigital() instead")
  void Start();

  /**
   * Stops closed-loop control. Note that closed loop control is enabled by
   * default.
   *
   * @deprecated Use Disable() instead.
   */
  WPI_DEPRECATED("Use Disable() instead")
  void Stop();

  /**
   * Check if compressor output is active.
   *
   * @return true if the compressor is on
   */
  bool Enabled() const;

  /**
   * Check if the pressure switch is triggered.
   *
   * @return true if pressure is low
   */
  bool GetPressureSwitchValue() const;

  /**
   * Query how much current the compressor is drawing.
   *
   * @return The current through the compressor, in amps
   */
  units::ampere_t GetCurrent() const;

  /**
   * Query the analog input voltage (on channel 0) (if supported).
   *
   * @return The analog input voltage, in volts
   */
  units::volt_t GetAnalogVoltage() const;

  /**
   * Query the analog sensor pressure (on channel 0) (if supported). Note this
   * is only for use with the REV Analog Pressure Sensor.
   *
   * @return The analog sensor pressure, in PSI
   */
  units::pounds_per_square_inch_t GetPressure() const;

  /**
   * Disable the compressor.
   */
  void Disable();

  /**
   * Enable compressor closed loop control using digital input.
   */
  void EnableDigital();

  /**
   * Enable compressor closed loop control using analog input. Note this is only
   * for use with the REV Analog Pressure Sensor.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI to enable compressor
   * @param maxPressure The maximum pressure in PSI to disable compressor
   */
  void EnableAnalog(units::pounds_per_square_inch_t minPressure,
                    units::pounds_per_square_inch_t maxPressure);

  /**
   * Enable compressor closed loop control using hybrid input. Note this is only
   * for use with the REV Analog Pressure Sensor.
   *
   * On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI to enable compressor
   * @param maxPressure The maximum pressure in PSI to disable compressor
   */
  void EnableHybrid(units::pounds_per_square_inch_t minPressure,
                    units::pounds_per_square_inch_t maxPressure);

  CompressorConfigType GetConfigType() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<PneumaticsBase> m_module;
};

}  // namespace frc
