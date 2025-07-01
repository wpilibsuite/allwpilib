// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "EdgeConfiguration.hpp"
#include "wpi/hal/Counter.h"
#include "wpi/hal/Types.h"
#include "wpi/telemetry/TelemetryLoggable.hpp"

namespace wpi {
/** Up Down Counter.
 *
 * This class can count edges on a single digital input or count up based on an
 * edge from one digital input and down on an edge from another digital input.
 *
 */
class UpDownCounter : public wpi::TelemetryLoggable {
 public:
  /**
   * Constructs a new UpDown Counter.
   *
   * @param channel The DIO channel
   * @param configuration Edge configuration
   */
  UpDownCounter(int channel, EdgeConfiguration configuration);

  UpDownCounter(UpDownCounter&&) = default;
  UpDownCounter& operator=(UpDownCounter&&) = default;

  ~UpDownCounter() override = default;

  /**
   * Gets the current count.
   *
   * @return The current count.
   */
  int GetCount() const;

  /** Resets the current count. */
  void Reset();

  /**
   * Sets the configuration for the channel.
   *
   * @param configuration The channel configuration.
   */
  void SetEdgeConfiguration(EdgeConfiguration configuration);

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  wpi::hal::Handle<HAL_CounterHandle, HAL_FreeCounter> m_handle;
  int32_t m_channel;
};
}  // namespace wpi
