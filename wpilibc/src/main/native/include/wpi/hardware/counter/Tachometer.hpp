// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/Counter.h"
#include "wpi/hardware/counter/EdgeConfiguration.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/frequency.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Handle.hpp"

namespace wpi {
/**
 * Tachometer for getting rotational velocity from a device.
 *
 * The Tachometer class measures the rate of digital pulses to determine the
 * rotation velocity of a mechanism. Examples of devices that could be used
 * with the tachometer class are a hall effect sensor, break beam sensor, or
 * optical sensor detecting tape on a shooter wheel. Unlike encoders, this class
 * only needs a single digital input.
 */
class Tachometer : public wpi::telemetry::TelemetryLoggable {
 public:
  /**
   * Constructs a new tachometer.
   *
   * @param channel The DIO Channel.
   * @param configuration Edge configuration
   */
  Tachometer(int channel, EdgeConfiguration configuration);

  Tachometer(Tachometer&&) = default;
  Tachometer& operator=(Tachometer&&) = default;

  ~Tachometer() override = default;

  /**
   * Sets the configuration for the channel.
   *
   * @param configuration The channel configuration.
   */
  void SetEdgeConfiguration(EdgeConfiguration configuration);

  /**
   * Sets the time window used to calculate the tachometer rate.
   *
   * @param window The rate calculation window. Valid values are 5 ms through
   *               255 ms. The default is 50 ms.
   */
  void SetRateWindow(wpi::units::milliseconds<> window);

  /**
   * Gets the tachometer frequency.
   *
   * @return Current frequency.
   */
  wpi::units::hertz<> GetFrequency() const;

  /**
   * Gets the number of edges per revolution.
   *
   * @return Edges per revolution.
   */
  int GetEdgesPerRevolution() const;

  /**
   * Sets the number of edges per revolution.
   *
   * @param edges Edges per revolution.
   */
  void SetEdgesPerRevolution(int edges);

  /**
   * Gets the current tachometer revolutions per second.
   *
   * SetEdgesPerRevolution must be set with a non 0 value for this to work.
   *
   * @return Current RPS.
   * @Common This is one of the commonly used methods for this class
   */
  wpi::units::turns_per_second<> GetRevolutionsPerSecond() const;

  /**
   * Gets the current tachometer revolutions per minute.
   *
   * SetEdgesPerRevolution must be set with a non 0 value for this to work.
   *
   * @return Current RPM.
   * @Common This is one of the commonly used methods for this class
   */
  wpi::units::revolutions_per_minute<> GetRevolutionsPerMinute() const;

  /**
   * Gets if the tachometer is stopped.
   *
   * The tachometer is stopped when its current frequency is zero.
   *
   * @return True if the tachometer is stopped.
   */
  bool GetStopped() const;

  void LogTo(wpi::telemetry::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  wpi::util::Handle<HAL_CounterHandle, HAL_FreeCounter> m_handle;
  int m_edgesPerRevolution{1};
  int32_t m_channel;
};
}  // namespace wpi
