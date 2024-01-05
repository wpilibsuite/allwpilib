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
/**
 * DMA sample.
 */
class DMASample : public HAL_DMASample {
 public:
  /**
   * DMA read status.
   */
  enum class DMAReadStatus {
    /// OK status.
    kOk = HAL_DMA_OK,
    /// Timeout status.
    kTimeout = HAL_DMA_TIMEOUT,
    /// Error status.
    kError = HAL_DMA_ERROR
  };

  /**
   * Retrieves a new DMA sample.
   *
   * @param dma DMA object.
   * @param timeout Timeout for retrieval.
   * @param remaining Number of remaining samples.
   * @param status DMA read status.
   */
  DMAReadStatus Update(const DMA* dma, units::second_t timeout,
                       int32_t* remaining, int32_t* status) {
    return static_cast<DMAReadStatus>(
        HAL_ReadDMA(dma->dmaHandle, this, timeout.value(), remaining, status));
  }

  /**
   * Returns the DMA sample time in microseconds.
   *
   * @return The DMA sample time in microseconds.
   */
  uint64_t GetTime() const { return timeStamp; }

  /**
   * Returns the DMA sample timestamp.
   *
   * @return The DMA sample timestamp.
   */
  units::second_t GetTimeStamp() const {
    return units::second_t{static_cast<double>(GetTime()) * 1.0e-6};
  }

  /**
   * Returns raw encoder value from DMA.
   *
   * @param encoder Encoder used for DMA.
   * @param status DMA read status.
   * @return Raw encoder value from DMA.
   */
  int32_t GetEncoderRaw(const Encoder* encoder, int32_t* status) const {
    return HAL_GetDMASampleEncoderRaw(this, encoder->m_encoder, status);
  }

  /**
   * Returns encoder distance from DMA.
   *
   * @param encoder Encoder used for DMA.
   * @param status DMA read status.
   * @return Encoder distance from DMA.
   */
  double GetEncoderDistance(const Encoder* encoder, int32_t* status) const {
    double val = GetEncoderRaw(encoder, status);
    val *= encoder->DecodingScaleFactor();
    val *= encoder->GetDistancePerPulse();
    return val;
  }

  /**
   * Returns raw encoder period from DMA.
   *
   * @param encoder Encoder used for DMA.
   * @param status DMA read status.
   * @return Raw encoder period from DMA.
   */
  int32_t GetEncoderPeriodRaw(const Encoder* encoder, int32_t* status) const {
    return HAL_GetDMASampleEncoderPeriodRaw(this, encoder->m_encoder, status);
  }

  /**
   * Returns counter value from DMA.
   *
   * @param counter Counter used for DMA.
   * @param status DMA read status.
   * @return Counter value from DMA.
   */
  int32_t GetCounter(const Counter* counter, int32_t* status) const {
    return HAL_GetDMASampleCounter(this, counter->m_counter, status);
  }

  /**
   * Returns counter period from DMA.
   *
   * @param counter Counter used for DMA.
   * @param status DMA read status.
   * @return Counter period from DMA.
   */
  int32_t GetCounterPeriod(const Counter* counter, int32_t* status) const {
    return HAL_GetDMASampleCounterPeriod(this, counter->m_counter, status);
  }

  /**
   * Returns digital source value from DMA.
   *
   * @param digitalSource DigitalSource used for DMA.
   * @param status DMA read status.
   * @return DigitalSource value from DMA.
   */
  bool GetDigitalSource(const DigitalSource* digitalSource,
                        int32_t* status) const {
    return HAL_GetDMASampleDigitalSource(
        this, digitalSource->GetPortHandleForRouting(), status);
  }

  /**
   * Returns raw analog input value from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @param status DMA read status.
   * @return Raw analog input value from DMA.
   */
  int32_t GetAnalogInputRaw(const AnalogInput* analogInput,
                            int32_t* status) const {
    return HAL_GetDMASampleAnalogInputRaw(this, analogInput->m_port, status);
  }

  /**
   * Returns analog input voltage from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @param status DMA read status.
   * @return Analog input voltage from DMA.
   */
  double GetAnalogInputVoltage(const AnalogInput* analogInput,
                               int32_t* status) {
    return HAL_GetAnalogValueToVolts(
        analogInput->m_port, GetAnalogInputRaw(analogInput, status), status);
  }

  /**
   * Returns averaged analog input raw value from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @param status DMA read status.
   * @return Averaged analog input raw value from DMA.
   */
  int32_t GetAveragedAnalogInputRaw(const AnalogInput* analogInput,
                                    int32_t* status) const {
    return HAL_GetDMASampleAveragedAnalogInputRaw(this, analogInput->m_port,
                                                  status);
  }

  /**
   * Returns averaged analog input voltage from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @param status DMA read status.
   * @return Averaged analog input voltage from DMA.
   */
  double GetAveragedAnalogInputVoltage(const AnalogInput* analogInput,
                                       int32_t* status) {
    return HAL_GetAnalogValueToVolts(
        analogInput->m_port, GetAveragedAnalogInputRaw(analogInput, status),
        status);
  }

  /**
   * Returns analog accumulator value from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @param count Accumulator sample count.
   * @param value Accumulator value.
   * @param status DMA read status.
   */
  void GetAnalogAccumulator(const AnalogInput* analogInput, int64_t* count,
                            int64_t* value, int32_t* status) const {
    return HAL_GetDMASampleAnalogAccumulator(this, analogInput->m_port, count,
                                             value, status);
  }

  /**
   * Returns raw duty cycle output from DMA.
   *
   * @param dutyCycle DutyCycle used for DMA.
   * @param status DMA read status.
   * @return Raw duty cycle output from DMA.
   */
  int32_t GetDutyCycleOutputRaw(const DutyCycle* dutyCycle,
                                int32_t* status) const {
    return HAL_GetDMASampleDutyCycleOutputRaw(this, dutyCycle->m_handle,
                                              status);
  }

  /**
   * Returns duty cycle output (0-1) from DMA.
   *
   * @param dutyCycle DutyCycle used for DMA.
   * @param status DMA read status.
   * @return Duty cycle output (0-1) from DMA.
   */
  double GetDutyCycleOutput(const DutyCycle* dutyCycle, int32_t* status) {
    return GetDutyCycleOutputRaw(dutyCycle, status) /
           static_cast<double>(dutyCycle->GetOutputScaleFactor());
  }
};

static_assert(std::is_standard_layout_v<frc::DMASample>,
              "frc::DMASample must have standard layout");
}  // namespace frc
