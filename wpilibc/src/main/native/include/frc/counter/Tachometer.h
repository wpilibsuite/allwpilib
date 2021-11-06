#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>
#include <units/frequency.h>
#include <units/time.h>
#include <units/angular_velocity.h>

namespace frc {
class DigitalSource;

class Tachometer
    : public wpi::Sendable,
      public wpi::SendableHelper<Tachometer> {
 public:
  Tachometer(DigitalSource& source);
  Tachometer(std::shared_ptr<DigitalSource> source);

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

  units::second_t GetMaxPeriod() const;
  void SetMaxPeriod(units::second_t maxPeriod);

  void SetUpdateWhenEmpty(bool updateWhenEmpty);

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void InitTachometer();
  std::shared_ptr<DigitalSource> m_source;
  hal::Handle<HAL_CounterHandle> m_handle;
  int m_edgesPerRevolution;
};
}