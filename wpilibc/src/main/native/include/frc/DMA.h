// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>

#include "frc/ErrorBase.h"

namespace frc {
class Encoder;
class Counter;
class DigitalSource;
class DutyCycle;
class AnalogInput;
class DMASample;

class DMA : public ErrorBase {
  friend class DMASample;

 public:
  DMA();
  ~DMA() override;

  DMA& operator=(DMA&& other) = default;
  DMA(DMA&& other) = default;

  void SetPause(bool pause);
  void SetRate(int cycles);

  void AddEncoder(const Encoder* encoder);
  void AddEncoderPeriod(const Encoder* encoder);

  void AddCounter(const Counter* counter);
  void AddCounterPeriod(const Counter* counter);

  void AddDigitalSource(const DigitalSource* digitalSource);

  void AddDutyCycle(const DutyCycle* digitalSource);

  void AddAnalogInput(const AnalogInput* analogInput);
  void AddAveragedAnalogInput(const AnalogInput* analogInput);
  void AddAnalogAccumulator(const AnalogInput* analogInput);

  void SetExternalTrigger(DigitalSource* source, bool rising, bool falling);

  void StartDMA(int queueDepth);
  void StopDMA();

 private:
  hal::Handle<HAL_DMAHandle> dmaHandle;
};
}  // namespace frc
