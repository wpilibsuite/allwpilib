/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Encoder.h"

namespace hal {
class Encoder {
 public:
  Encoder(uint8_t port_a_module, uint32_t port_a_pin,
          bool port_a_analog_trigger, uint8_t port_b_module,
          uint32_t port_b_pin, bool port_b_analog_trigger,
          bool reverseDirection, EncoderEncodingType encodingType,
          int32_t* status);
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
  void SetSamplesToAverage(int samplesToAverage, int32_t* status);
  int32_t GetSamplesToAverage(int32_t* status) const;

  void SetIndexSource(uint32_t pin, bool analogTrigger,
                      EncoderIndexingType type, int32_t* status);

  int32_t GetFPGAIndex() const { return m_index; }

  int32_t GetEncodingScale() const { return m_encodingScale; }

  double DecodingScaleFactor() const;

  double GetDistancePerPulse() const { return m_distancePerPulse; }

  EncoderEncodingType GetEncodingType() const { return m_encodingType; }

 private:
  void SetupCounter(uint8_t port_a_module, uint32_t port_a_pin,
                    bool port_a_analog_trigger, uint8_t port_b_module,
                    uint32_t port_b_pin, bool port_b_analog_trigger,
                    bool reverseDirection, EncoderEncodingType encodingType,
                    int32_t* status);

  HalFPGAEncoderHandle m_encoder = HAL_INVALID_HANDLE;

  HalCounterHandle m_counter = HAL_INVALID_HANDLE;

  int32_t m_index = 0;

  double m_distancePerPulse = 1.0;

  EncoderEncodingType m_encodingType;

  int32_t m_encodingScale;
};
}
