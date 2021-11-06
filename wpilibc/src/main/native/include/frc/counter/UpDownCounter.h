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

class UpDownCounter : public wpi::Sendable,
                      public wpi::SendableHelper<UpDownCounter> {
 public:
  UpDownCounter(DigitalSource& upSource, DigitalSource& downSource);
  UpDownCounter(std::shared_ptr<DigitalSource> countSource,
                std::shared_ptr<DigitalSource> directionSource);

  ~UpDownCounter() override;

  UpDownCounter(UpDownCounter&&) = default;
  UpDownCounter& operator=(UpDownCounter&&) = default;

  int GetCount() const;

  void SetReverseDirection(bool reverseDirection);
  void Reset();

  void SetUpEdgeConfiguration(EdgeConfiguration configuration);
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
