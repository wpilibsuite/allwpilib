// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "EdgeConfiguration.h"

namespace frc {
class DigitalSource;

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
   * @param upSource The up count source (can be null).
   * @param downSource The down count source (can be null).
   */
  UpDownCounter(DigitalSource& upSource, DigitalSource& downSource);

  /**
   * Constructs a new UpDown Counter.
   *
   * @param upSource The up count source (can be null).
   * @param downSource The down count source (can be null).
   */
  UpDownCounter(std::shared_ptr<DigitalSource> upSource,
                std::shared_ptr<DigitalSource> downSource);

  ~UpDownCounter() override;

  UpDownCounter(UpDownCounter&&) = default;
  UpDownCounter& operator=(UpDownCounter&&) = default;

  /**
   * Gets the current count.
   *
   * @return The current count.
   */
  int GetCount() const;

  /**
   * Sets to revert the counter direction.
   *
   * @param reverseDirection True to reverse counting direction.
   */
  void SetReverseDirection(bool reverseDirection);

  /** Resets the current count. */
  void Reset();

  /**
   * Sets the configuration for the up source.
   *
   * @param configuration The up source configuration.
   */
  void SetUpEdgeConfiguration(EdgeConfiguration configuration);

  /**
   * Sets the configuration for the down source.
   *
   * @param configuration The down source configuration.
   */
  void SetDownEdgeConfiguration(EdgeConfiguration configuration);

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void InitUpDownCounter();
  std::shared_ptr<DigitalSource> m_upSource;
  std::shared_ptr<DigitalSource> m_downSource;
  hal::Handle<HAL_CounterHandle> m_handle;
  int32_t m_index = 0;
};
}  // namespace frc
