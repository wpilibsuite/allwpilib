#pragma once

#include "hal/DMA.h"
#include "frc/DMA.h"
#include "units/units.h"
#include "frc/Encoder.h"
#include "frc/Counter.h"
#include "frc/AnalogInput.h"
#include "frc/DutyCycle.h"

namespace frc {
class DMASample : public HAL_DMASample {
 public:
  HAL_DMAReadStatus Update(DMA* dma, units::second_t timeout,
                           int32_t* remaining, int32_t* status) {
    units::millisecond_t ms = timeout;
    auto timeoutMs = ms.to<int32_t>();
    return HAL_ReadDMA(dma->dmaHandle, this, timeoutMs, remaining, status);
  }

  units::microsecond_t GetTime() const {
    return units::microsecond_t{timeStamp};
  }

  int32_t GetEncoder(const Encoder* encoder, int32_t* status) const {
    return HAL_GetDMASampleEncoder(this, encoder->m_encoder, status);
  }

  int32_t GetEncoderRate(const Encoder* encoder, int32_t* status) const {
    return HAL_GetDMASampleEncoderRate(this, encoder->m_encoder, status);
  }

  int32_t GetCounter(const Counter* counter, int32_t* status) const {
    return HAL_GetDMASampleCounter(this, counter->m_counter, status);
  }

  int32_t GetCounterRate(const Counter* counter, int32_t* status) const {
    return HAL_GetDMASampleCounterRate(this, counter->m_counter, status);
  }

  bool GetDigitalSource(const DigitalSource* digitalSource,
                        int32_t* status) const {
    return HAL_GetDMASampleDigitalSource(
        this, digitalSource->GetPortHandleForRouting(), status);
  }

  int32_t GetAnalogInput(const AnalogInput* analogInput,
                         int32_t* status) const {
    return HAL_GetDMASampleAnalogInput(this, analogInput->m_port, status);
  }

  int32_t GetAveragedAnalogInput(const AnalogInput* analogInput,
                                 int32_t* status) const {
    return HAL_GetDMASampleAveragedAnalogInput(this, analogInput->m_port,
                                               status);
  }

  int64_t GetAnalogAccumulatorCount(const AnalogInput* analogInput,
                                    int32_t* status) const {
    return HAL_GetDMASampleAnalogAccumulatorCount(this, analogInput->m_port,
                                                  status);
  }

  int64_t GetAnalogAccumulatorValue(const AnalogInput* analogInput,
                                    int32_t* status) const {
    return HAL_GetDMASampleAnalogAccumulatorValue(this, analogInput->m_port,
                                                  status);
  }

  int32_t GetDutyCycleOutput(const DutyCycle* dutyCycle,
                             int32_t* status) const {
    return HAL_GetDMASampleDutyCycleOutput(this, dutyCycle->m_handle, status);
  }
};

static_assert(std::is_pod_v<frc::DMASample>, "DMA Sample MUST Be POD");
}  // namespace frc
