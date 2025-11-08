// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/hal/Counter.h"
#include "wpi/hal/Types.h"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

#include "EdgeConfiguration.hpp"

namespace frc {
/** Up Down Counter.
 *
 * This class can count edges on a single digital input or count up based on an
 * edge from one digital input and down on an edge from another digital input.
 *
 */
class UpDownCounter : public wpi::Sendable,
                      public wpi::SendableHelper<UpDownCounter> {
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

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_CounterHandle, HAL_FreeCounter> m_handle;
  int32_t m_channel;
};
}  // namespace frc
