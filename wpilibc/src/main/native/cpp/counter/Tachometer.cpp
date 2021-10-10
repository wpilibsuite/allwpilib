#include "frc/counter/Tachometer.h"
#include <hal/Counter.h>
#include "frc/Errors.h"
#include <frc/DigitalSource.h>

using namespace frc;

Tachometer::Tachometer(DigitalSource& source) {}
Tachometer::Tachometer(DigitalSource* source) {}
Tachometer::Tachometer(std::shared_ptr<DigitalSource> source) {}

Tachometer::~Tachometer() {}

units::hertz_t Tachometer::GetFrequency() const {
  auto period = GetPeriod();
  if (period.to<double>() == 0) {
    return units::hertz_t{0.0};
  }
  return 1 / period;
}

units::second_t Tachometer::GetPeriod() const {
  int32_t status = 0;
  double period = HAL_GetCounterPeriod(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
  return units::second_t{period};
}

int Tachometer::GetEdgesPerRevolution() const {
  return m_edgesPerRevolution;
}
void Tachometer::SetEdgesPerRevolution(int edges) {
  m_edgesPerRevolution = edges;
}

units::revolutions_per_minute_t Tachometer::GetRevolutionsPerMinute() const {
  auto period = GetPeriod();
  if (period.to<double>() == 0) {
    return units::revolutions_per_minute_t{0.0};
  }
  int edgesPerRevolution = GetEdgesPerRevolution();
  if (edgesPerRevolution == 0) {
    return units::revolutions_per_minute_t{0.0};
  }
  return units::revolutions_per_minute_t{((1.0 / edgesPerRevolution) / period) * 60};
}

bool Tachometer::GetStopped() const {}

int Tachometer::GetSamplesToAverage() const {}
void Tachometer::SetSamplesToAverage(int samples) {}

units::second_t Tachometer::GetMaxPeriod() const {}
void Tachometer::SetMaxPeriod(units::second_t maxPeriod) {}

void Tachometer::SetUpdateWhenEmpty(bool updateWhenEmpty) {}

void Tachometer::InitSendable(wpi::SendableBuilder& builder) {}

void Tachometer::InitTachometer() {}