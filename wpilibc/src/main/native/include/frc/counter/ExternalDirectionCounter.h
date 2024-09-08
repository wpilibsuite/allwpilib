// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Counter.h>
#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "EdgeConfiguration.h"

namespace frc {
class DigitalSource;

/** Counter using external direction.
 *
 * <p>This counts on an edge from one digital input and the whether it counts
 * up or down based on the state of a second digital input.
 *
 */
class ExternalDirectionCounter
    : public wpi::Sendable,
      public wpi::SendableHelper<ExternalDirectionCounter> {
 public:
  /**
   * Constructs a new ExternalDirectionCounter.
   *
   * @param countSource The source for counting.
   * @param directionSource The source for selecting count direction.
   */
  ExternalDirectionCounter(DigitalSource& countSource,
                           DigitalSource& directionSource);

  /**
   * Constructs a new ExternalDirectionCounter.
   *
   * @param countSource The source for counting.
   * @param directionSource The source for selecting count direction.
   */
  ExternalDirectionCounter(std::shared_ptr<DigitalSource> countSource,
                           std::shared_ptr<DigitalSource> directionSource);

  ExternalDirectionCounter(ExternalDirectionCounter&&) = default;
  ExternalDirectionCounter& operator=(ExternalDirectionCounter&&) = default;

  ~ExternalDirectionCounter() override = default;

  /**
   * Gets the current count.
   *
   * @return The current count.
   */
  int GetCount() const;

  /**
   * Sets to reverse the counter direction.
   *
   * @param reverseDirection True to reverse counting direction.
   */
  void SetReverseDirection(bool reverseDirection);

  /** Resets the current count. */
  void Reset();

  /**
   * Sets the edge configuration for counting.
   *
   * @param configuration The counting edge configuration.
   */
  void SetEdgeConfiguration(EdgeConfiguration configuration);

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<DigitalSource> m_countSource;
  std::shared_ptr<DigitalSource> m_directionSource;
  hal::Handle<HAL_CounterHandle, HAL_FreeCounter> m_handle;
  int32_t m_index = 0;
};
}  // namespace frc
