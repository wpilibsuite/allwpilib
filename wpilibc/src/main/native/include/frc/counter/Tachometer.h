// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Counter.h>
#include <hal/Types.h>
#include <units/angular_velocity.h>
#include <units/frequency.h>
#include <units/time.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "EdgeConfiguration.h"

namespace frc {
/**
 * Tachometer for getting rotational speed from a device.
 *
 * <p>The Tachometer class measures the time between digital pulses to
 * determine the rotation speed of a mechanism. Examples of devices that could
 * be used with the tachometer class are a hall effect sensor, break beam
 * sensor, or optical sensor detecting tape on a shooter wheel. Unlike
 * encoders, this class only needs a single digital input.
 */
class Tachometer : public wpi::Sendable,
                   public wpi::SendableHelper<Tachometer> {
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
   * Gets the tachometer frequency.
   *
   * @return Current frequency.
   */
  units::hertz_t GetFrequency() const;

  /**
   * Gets the tachometer period.
   *
   * @return Current period.
   */
  units::second_t GetPeriod() const;

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
   */
  units::turns_per_second_t GetRevolutionsPerSecond() const;

  /**
   * Gets the current tachometer revolutions per minute.
   *
   * SetEdgesPerRevolution must be set with a non 0 value for this to work.
   *
   * @return Current RPM.
   */
  units::revolutions_per_minute_t GetRevolutionsPerMinute() const;

  /**
   * Gets if the tachometer is stopped.
   *
   * @return True if the tachometer is stopped.
   */
  bool GetStopped() const;

  /**
   * Sets the maximum period before the tachometer is considered stopped.
   *
   * @param maxPeriod The max period.
   */
  void SetMaxPeriod(units::second_t maxPeriod);

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_CounterHandle, HAL_FreeCounter> m_handle;
  int m_edgesPerRevolution;
  int32_t m_channel;
};
}  // namespace frc
