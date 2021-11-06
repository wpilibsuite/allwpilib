// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <units/angular_velocity.h>
#include <units/frequency.h>
#include <units/time.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {
class DigitalSource;

class Tachometer : public wpi::Sendable,
                   public wpi::SendableHelper<Tachometer> {
 public:
  explicit Tachometer(DigitalSource& source);
  explicit Tachometer(std::shared_ptr<DigitalSource> source);

  ~Tachometer() override;

  Tachometer(Tachometer&&) = default;
  Tachometer& operator=(Tachometer&&) = default;

  units::hertz_t GetFrequency() const;
  units::second_t GetPeriod() const;

  int GetEdgesPerRevolution() const;
  void SetEdgesPerRevolution(int edges);

  units::revolutions_per_minute_t GetRevolutionsPerMinute() const;

  bool GetStopped() const;

  int GetSamplesToAverage() const;
  void SetSamplesToAverage(int samples);

  void SetMaxPeriod(units::second_t maxPeriod);

  void SetUpdateWhenEmpty(bool updateWhenEmpty);

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<DigitalSource> m_source;
  hal::Handle<HAL_CounterHandle> m_handle;
  int m_edgesPerRevolution;
  int32_t m_index;
};
}  // namespace frc
