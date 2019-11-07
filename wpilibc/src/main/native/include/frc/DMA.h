#pragma once

#include "hal/Types.h"

#include "frc/ErrorBase.h"

namespace frc {
class Encoder;
class Counter;
class DigitalSource;
class DutyCycle;
class AnalogInput;

class DMA : ErrorBase {
 public:
  DMA();
  ~DMA() override;

  DMA& operator=(DMA&& other) = default;
  DMA(DMA&& other) = default;

  void SetPause(bool pause);
  void SetRate(int cycles);

  void AddEncoder(const Encoder* encoder);
  void AddEncoderRate(const Encoder* encoder);

  void AddCounter(const Counter* counter);
  void AddCounterRate(const Counter* counter);

  void AddDigitalSource(const DigitalSource* digitalSource);

  void AddAnalogInput(const AnalogInput* analogInput);
  void AddAveragedAnalogInput(const AnalogInput* analogInput);
  void AddAnalogAccumulator(const AnalogInput* analogInput);

  void SetExternalTrigger(DigitalSource source, bool rising, bool falling);

  void StartDMA(int queueDepth);
  void StopDMA();

 private:
  hal::Handle<HAL_DMAHandle> dmaHandle;
};
}
