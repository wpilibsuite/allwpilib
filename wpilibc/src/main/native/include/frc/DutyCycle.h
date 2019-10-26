#pragma once

#include <memory>

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {
class DigitalSource;
class AnalogTrigger;

class DutyCycle : public ErrorBase, public Sendable, public SendableHelper<DutyCycle> {
  friend class AnalogTrigger;
public:
  DutyCycle(DigitalSource& source);
  DutyCycle(DigitalSource* source);
  DutyCycle(std::shared_ptr<DigitalSource> source);

  ~DutyCycle() override;

  DutyCycle(DutyCycle&&) = default;
  DutyCycle& operator=(DutyCycle&&) = default;

  int GetFrequency() const;
  double GetOutput() const;
  unsigned int GetOutputRaw() const;
  unsigned int GetOutputScaleFactor() const;

  void InitSendable(SendableBuilder& builder) override;
private:
  void InitDutyCycle();
  std::shared_ptr<DigitalSource> m_source;
  hal::Handle<HAL_DutyCycleHandle> m_handle;
};
}
