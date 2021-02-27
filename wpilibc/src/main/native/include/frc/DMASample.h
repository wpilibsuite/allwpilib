// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include <hal/AnalogInput.h>
#include <hal/DMA.h>
#include <units/time.h>

#include "frc/AnalogInput.h"
#include "frc/Counter.h"
#include "frc/DMA.h"
#include "frc/DutyCycle.h"
#include "frc/Encoder.h"

namespace frc {
class DMASample : public HAL_DMASample {
 public:
  HAL_DMAReadStatus Update(const DMA* dma, units::second_t timeout,
                           int32_t* remaining, int32_t* status) {
    units::millisecond_t ms = timeout;
    auto timeoutMs = ms.to<int32_t>();
    return HAL_ReadDMA(dma->dmaHandle, this, timeoutMs, remaining, status);
  }

  uint64_t GetTime() const { return timeStamp; }

  units::second_t GetTimeStamp() const {
    return units::second_t{static_cast<double>(GetTime()) * 1.0e-6};
  }

  int32_t GetEncoderRaw(const Encoder* encoder, int32_t* status) const {
    return HAL_GetDMASampleEncoderRaw(this, encoder->m_encoder, status);
  }

  double GetEncoderDistance(const Encoder* encoder, int32_t* status) const {
    double val = GetEncoderRaw(encoder, status);
    val *= encoder->DecodingScaleFactor();
    val *= encoder->GetDistancePerPulse();
    return val;
  }

  int32_t GetEncoderPeriodRaw(const Encoder* encoder, int32_t* status) const {
    return HAL_GetDMASampleEncoderPeriodRaw(this, encoder->m_encoder, status);
  }

  int32_t GetCounter(const Counter* counter, int32_t* status) const {
    return HAL_GetDMASampleCounter(this, counter->m_counter, status);
  }

  int32_t GetCounterPeriod(const Counter* counter, int32_t* status) const {
    return HAL_GetDMASampleCounterPeriod(this, counter->m_counter, status);
  }

  bool GetDigitalSource(const DigitalSource* digitalSource,
                        int32_t* status) const {
    return HAL_GetDMASampleDigitalSource(
        this, digitalSource->GetPortHandleForRouting(), status);
  }

  int32_t GetAnalogInputRaw(const AnalogInput* analogInput,
                            int32_t* status) const {
    return HAL_GetDMASampleAnalogInputRaw(this, analogInput->m_port, status);
  }

  double GetAnalogInputVoltage(const AnalogInput* analogInput,
                               int32_t* status) {
    return HAL_GetAnalogValueToVolts(
        analogInput->m_port, GetAnalogInputRaw(analogInput, status), status);
  }

  int32_t GetAveragedAnalogInputRaw(const AnalogInput* analogInput,
                                    int32_t* status) const {
    return HAL_GetDMASampleAveragedAnalogInputRaw(this, analogInput->m_port,
                                                  status);
  }

  double GetAveragedAnalogInputVoltage(const AnalogInput* analogInput,
                                       int32_t* status) {
    return HAL_GetAnalogValueToVolts(
        analogInput->m_port, GetAveragedAnalogInputRaw(analogInput, status),
        status);
  }

  void GetAnalogAccumulator(const AnalogInput* analogInput, int64_t* count,
                            int64_t* value, int32_t* status) const {
    return HAL_GetDMASampleAnalogAccumulator(this, analogInput->m_port, count,
                                             value, status);
  }

  int32_t GetDutyCycleOutputRaw(const DutyCycle* dutyCycle,
                                int32_t* status) const {
    return HAL_GetDMASampleDutyCycleOutputRaw(this, dutyCycle->m_handle,
                                              status);
  }

  double GetDutyCycleOutput(const DutyCycle* dutyCycle, int32_t* status) {
    return GetDutyCycleOutputRaw(dutyCycle, status) /
           static_cast<double>(dutyCycle->GetOutputScaleFactor());
  }
};

static_assert(std::is_standard_layout_v<frc::DMASample>,
              "frc::DMASample must have standard layout");
}  // namespace frc
