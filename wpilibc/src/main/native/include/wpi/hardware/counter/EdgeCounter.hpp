// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/Counter.h"
#include "wpi/hardware/counter/EdgeConfiguration.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/util/Handle.hpp"

namespace wpi {
/** Counts rising or falling edges on a single digital input. */
class EdgeCounter : public wpi::telemetry::TelemetryLoggable {
 public:
  /**
   * Constructs a new edge counter.
   *
   * @param channel The DIO channel
   * @param configuration Edge configuration
   */
  EdgeCounter(int channel, EdgeConfiguration configuration);

  EdgeCounter(EdgeCounter&&) = default;
  EdgeCounter& operator=(EdgeCounter&&) = default;

  ~EdgeCounter() override = default;

  /**
   * Gets the current count.
   *
   * @return The current count.
   * @Common This is one of the commonly used methods for this class
   */
  int GetCount() const;

  /**
   * Resets the current count.
   *
   * @Common This is one of the commonly used methods for this class
   */
  void Reset();

  /**
   * Sets the configuration for the channel.
   *
   * @param configuration The channel configuration.
   */
  void SetEdgeConfiguration(EdgeConfiguration configuration);

  void LogTo(wpi::telemetry::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  wpi::util::Handle<HAL_CounterHandle, HAL_FreeCounter> m_handle;
  int32_t m_channel;
};
}  // namespace wpi
