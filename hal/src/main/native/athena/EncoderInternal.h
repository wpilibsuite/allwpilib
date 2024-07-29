// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Encoder.h"

namespace hal {

bool GetEncoderBaseHandle(HAL_EncoderHandle handle,
                          HAL_FPGAEncoderHandle* fpgaEncoderHandle,
                          HAL_CounterHandle* counterHandle);

class Encoder {
 public:
  friend bool GetEncoderBaseHandle(HAL_EncoderHandle handle,
                                   HAL_FPGAEncoderHandle* fpgaEncoderHandle,
                                   HAL_CounterHandle* counterHandle);

  Encoder(HAL_Handle digitalSourceHandleA,
          HAL_AnalogTriggerType analogTriggerTypeA,
          HAL_Handle digitalSourceHandleB,
          HAL_AnalogTriggerType analogTriggerTypeB, bool reverseDirection,
          HAL_EncoderEncodingType encodingType, int32_t* status);
  ~Encoder();

  // CounterBase interface
  int32_t Get(int32_t* status) const;
  int32_t GetRaw(int32_t* status) const;
  int32_t GetEncodingScale(int32_t* status) const;
  void Reset(int32_t* status);
  double GetPeriod(int32_t* status) const;
  void SetMaxPeriod(double maxPeriod, int32_t* status);
  bool GetStopped(int32_t* status) const;
  bool GetDirection(int32_t* status) const;

  double GetDistance(int32_t* status) const;
  double GetRate(int32_t* status) const;
  void SetMinRate(double minRate, int32_t* status);
  void SetDistancePerPulse(double distancePerPulse, int32_t* status);
  void SetReverseDirection(bool reverseDirection, int32_t* status);
  void SetSamplesToAverage(int32_t samplesToAverage, int32_t* status);
  int32_t GetSamplesToAverage(int32_t* status) const;

  void SetIndexSource(HAL_Handle digitalSourceHandle,
                      HAL_AnalogTriggerType analogTriggerType,
                      HAL_EncoderIndexingType type, int32_t* status);

  int32_t GetFPGAIndex() const { return m_index; }

  int32_t GetEncodingScale() const { return m_encodingScale; }

  double DecodingScaleFactor() const;

  double GetDistancePerPulse() const { return m_distancePerPulse; }

  HAL_EncoderEncodingType GetEncodingType() const { return m_encodingType; }

 private:
  void SetupCounter(HAL_Handle digitalSourceHandleA,
                    HAL_AnalogTriggerType analogTriggerTypeA,
                    HAL_Handle digitalSourceHandleB,
                    HAL_AnalogTriggerType analogTriggerTypeB,
                    bool reverseDirection, HAL_EncoderEncodingType encodingType,
                    int32_t* status);

  HAL_FPGAEncoderHandle m_encoder = HAL_kInvalidHandle;

  HAL_CounterHandle m_counter = HAL_kInvalidHandle;

  int32_t m_index = 0;

  double m_distancePerPulse = 1.0;

  HAL_EncoderEncodingType m_encodingType;

  int32_t m_encodingScale;
};

}  // namespace hal
