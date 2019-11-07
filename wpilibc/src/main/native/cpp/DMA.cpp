#include "frc/DMA.h"

#include "hal/DMA.h"
#include "hal/HALBase.h"

using namespace frc;

  DMA::DMA() {
    int32_t status = 0;
    dmaHandle = HAL_InitializeDMA(&status);
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  }

  DMA::~DMA() {

  }

  void DMA::SetPause(bool pause);
  void DMA::SetRate(int cycles);

  void DMA::AddEncoder(const Encoder* encoder);
  void DMA::AddEncoderRate(const Encoder* encoder);

  void DMA::AddCounter(const Counter* counter);
  void DMA::AddCounterRate(const Counter* counter);

  void DMA::AddDigitalSource(const DigitalSource* digitalSource);

  void DMA::AddAnalogInput(const AnalogInput* analogInput);
  void DMA::AddAveragedAnalogInput(const AnalogInput* analogInput);
  void DMA::AddAnalogAccumulator(const AnalogInput* analogInput);

  void DMA::SetExternalTrigger(DigitalSource source, bool rising, bool falling);

  void DMA::StartDMA(int queueDepth);
  void DMA::StopDMA();
