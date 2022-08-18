// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DMA.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

#include "AnalogInternal.h"
#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "EncoderInternal.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/AnalogAccumulator.h"
#include "hal/AnalogGyro.h"
#include "hal/AnalogInput.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

static_assert(std::is_standard_layout_v<HAL_DMASample>,
              "HAL_DMASample must have standard layout");

namespace {

struct DMA {
  std::unique_ptr<tDMAManager> manager;
  std::unique_ptr<tDMA> aDMA;

  HAL_DMASample captureStore;
};
}  // namespace

static constexpr size_t kChannelSize[22] = {2, 2, 4, 4, 2, 2, 4, 4, 3, 3, 2,
                                            1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

enum DMAOffsetConstants {
  kEnable_AI0_Low = 0,
  kEnable_AI0_High = 1,
  kEnable_AIAveraged0_Low = 2,
  kEnable_AIAveraged0_High = 3,
  kEnable_AI1_Low = 4,
  kEnable_AI1_High = 5,
  kEnable_AIAveraged1_Low = 6,
  kEnable_AIAveraged1_High = 7,
  kEnable_Accumulator0 = 8,
  kEnable_Accumulator1 = 9,
  kEnable_DI = 10,
  kEnable_AnalogTriggers = 11,
  kEnable_Counters_Low = 12,
  kEnable_Counters_High = 13,
  kEnable_CounterTimers_Low = 14,
  kEnable_CounterTimers_High = 15,
  kEnable_Encoders_Low = 16,
  kEnable_Encoders_High = 17,
  kEnable_EncoderTimers_Low = 18,
  kEnable_EncoderTimers_High = 19,
  kEnable_DutyCycle_Low = 20,
  kEnable_DutyCycle_High = 21,
};

static hal::LimitedHandleResource<HAL_DMAHandle, DMA, 1, HAL_HandleEnum::DMA>*
    dmaHandles;

namespace hal::init {
void InitializeDMA() {
  static hal::LimitedHandleResource<HAL_DMAHandle, DMA, 1, HAL_HandleEnum::DMA>
      dH;
  dmaHandles = &dH;
}
}  // namespace hal::init

extern "C" {

HAL_DMAHandle HAL_InitializeDMA(int32_t* status) {
  HAL_Handle handle = dmaHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto dma = dmaHandles->Get(handle);

  if (!dma) {
    // Can only happen on thread error
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  // Manager does not get created until DMA is started
  dma->aDMA.reset(tDMA::create(status));
  if (*status != 0) {
    dmaHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  std::memset(&dma->captureStore, 0, sizeof(dma->captureStore));

  tDMA::tConfig config;
  std::memset(&config, 0, sizeof(config));
  config.Pause = true;
  dma->aDMA->writeConfig(config, status);

  dma->aDMA->writeRate(1, status);

  tDMA::tExternalTriggers newTrigger;
  std::memset(&newTrigger, 0, sizeof(newTrigger));
  for (unsigned char reg = 0; reg < tDMA::kNumExternalTriggersRegisters;
       reg++) {
    for (unsigned char bit = 0; bit < tDMA::kNumExternalTriggersElements;
         bit++) {
      dma->aDMA->writeExternalTriggers(reg, bit, newTrigger, status);
    }
  }

  return handle;
}

void HAL_FreeDMA(HAL_DMAHandle handle) {
  auto dma = dmaHandles->Get(handle);
  dmaHandles->Free(handle);

  if (!dma) {
    return;
  }

  int32_t status = 0;
  if (dma->manager) {
    dma->manager->stop(&status);
  }
}

void HAL_SetDMAPause(HAL_DMAHandle handle, HAL_Bool pause, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (!dma->manager) {
    *status = HAL_INVALID_DMA_STATE;
    return;
  }

  dma->aDMA->writeConfig_Pause(pause, status);
}

void HAL_SetDMATimedTrigger(HAL_DMAHandle handle, double seconds,
                            int32_t* status) {
  constexpr double baseMultipler = kSystemClockTicksPerMicrosecond * 1000000;
  uint32_t cycles = static_cast<uint32_t>(baseMultipler * seconds);
  HAL_SetDMATimedTriggerCycles(handle, cycles, status);
}

void HAL_SetDMATimedTriggerCycles(HAL_DMAHandle handle, uint32_t cycles,
                                  int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (cycles < 1) {
    cycles = 1;
  }

  dma->aDMA->writeConfig_ExternalClock(false, status);
  if (*status != 0) {
    return;
  }

  dma->aDMA->writeRate(cycles, status);
}

void HAL_AddDMAEncoder(HAL_DMAHandle handle, HAL_EncoderHandle encoderHandle,
                       int32_t* status) {
  // Detect a counter encoder vs an actual encoder, and use the right DMA calls
  HAL_FPGAEncoderHandle fpgaEncoderHandle = HAL_kInvalidHandle;
  HAL_CounterHandle counterHandle = HAL_kInvalidHandle;

  bool validEncoderHandle = hal::GetEncoderBaseHandle(
      encoderHandle, &fpgaEncoderHandle, &counterHandle);

  if (!validEncoderHandle) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (counterHandle != HAL_kInvalidHandle) {
    HAL_AddDMACounter(handle, counterHandle, status);
    return;
  }

  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(fpgaEncoderHandle) != HAL_HandleEnum::FPGAEncoder) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(fpgaEncoderHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_Encoders_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_Encoders_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMAEncoderPeriod(HAL_DMAHandle handle,
                             HAL_EncoderHandle encoderHandle, int32_t* status) {
  // Detect a counter encoder vs an actual encoder, and use the right DMA calls
  HAL_FPGAEncoderHandle fpgaEncoderHandle = HAL_kInvalidHandle;
  HAL_CounterHandle counterHandle = HAL_kInvalidHandle;

  bool validEncoderHandle = hal::GetEncoderBaseHandle(
      encoderHandle, &fpgaEncoderHandle, &counterHandle);

  if (!validEncoderHandle) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (counterHandle != HAL_kInvalidHandle) {
    HAL_AddDMACounterPeriod(handle, counterHandle, status);
    return;
  }

  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(fpgaEncoderHandle) != HAL_HandleEnum::FPGAEncoder) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(fpgaEncoderHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_EncoderTimers_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_EncoderTimers_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMACounter(HAL_DMAHandle handle, HAL_CounterHandle counterHandle,
                       int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(counterHandle) != HAL_HandleEnum::Counter) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(counterHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_Counters_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_Counters_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMACounterPeriod(HAL_DMAHandle handle,
                             HAL_CounterHandle counterHandle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(counterHandle) != HAL_HandleEnum::Counter) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(counterHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_CounterTimers_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_CounterTimers_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMADigitalSource(HAL_DMAHandle handle,
                             HAL_Handle digitalSourceHandle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (isHandleType(digitalSourceHandle, HAL_HandleEnum::AnalogTrigger)) {
    dma->aDMA->writeConfig_Enable_AnalogTriggers(true, status);
  } else if (isHandleType(digitalSourceHandle, HAL_HandleEnum::DIO)) {
    dma->aDMA->writeConfig_Enable_DI(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMAAnalogInput(HAL_DMAHandle handle,
                           HAL_AnalogInputHandle aInHandle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(aInHandle) != HAL_HandleEnum::AnalogInput) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(aInHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_AI0_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_AI0_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMADutyCycle(HAL_DMAHandle handle,
                         HAL_DutyCycleHandle dutyCycleHandle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(dutyCycleHandle) != HAL_HandleEnum::DutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(dutyCycleHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_DutyCycle_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_DutyCycle_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMAAveragedAnalogInput(HAL_DMAHandle handle,
                                   HAL_AnalogInputHandle aInHandle,
                                   int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (getHandleType(aInHandle) != HAL_HandleEnum::AnalogInput) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t index = getHandleIndex(aInHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index < 4) {
    dma->aDMA->writeConfig_Enable_AIAveraged0_Low(true, status);
  } else if (index < 8) {
    dma->aDMA->writeConfig_Enable_AIAveraged0_High(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

void HAL_AddDMAAnalogAccumulator(HAL_DMAHandle handle,
                                 HAL_AnalogInputHandle aInHandle,
                                 int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return;
  }

  if (!HAL_IsAccumulatorChannel(aInHandle, status)) {
    *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
    return;
  }

  int32_t index = getHandleIndex(aInHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index == 0) {
    dma->aDMA->writeConfig_Enable_Accumulator0(true, status);
  } else if (index == 1) {
    dma->aDMA->writeConfig_Enable_Accumulator1(true, status);
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
}

int32_t HAL_SetDMAExternalTrigger(HAL_DMAHandle handle,
                                  HAL_Handle digitalSourceHandle,
                                  HAL_AnalogTriggerType analogTriggerType,
                                  HAL_Bool rising, HAL_Bool falling,
                                  int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_ADDITION;
    return 0;
  }

  int index = 0;
  auto triggerChannels = dma->captureStore.triggerChannels;
  do {
    if (((triggerChannels >> index) & 0x1) == 0) {
      break;
    }
    index++;
  } while (index < 8);

  if (index == 8) {
    *status = NO_AVAILABLE_RESOURCES;
    return 0;
  }

  dma->captureStore.triggerChannels |= (1 << index);

  auto channelIndex = index;

  dma->aDMA->writeConfig_ExternalClock(true, status);
  if (*status != 0) {
    return 0;
  }

  dma->aDMA->writeRate(1, status);
  if (*status != 0) {
    return 0;
  }

  uint8_t pin = 0;
  uint8_t module = 0;
  bool analogTrigger = false;
  bool success = remapDigitalSource(digitalSourceHandle, analogTriggerType, pin,
                                    module, analogTrigger);

  if (!success) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(status,
                      "Digital Source unabled to be mapped properly. Likely "
                      "invalid handle passed.");
    return 0;
  }

  tDMA::tExternalTriggers newTrigger;
  newTrigger.FallingEdge = falling;
  newTrigger.RisingEdge = rising;
  newTrigger.ExternalClockSource_AnalogTrigger = analogTrigger;
  newTrigger.ExternalClockSource_Channel = pin;
  newTrigger.ExternalClockSource_Module = module;

  dma->aDMA->writeExternalTriggers(channelIndex / 4, channelIndex % 4,
                                   newTrigger, status);
  return index;
}

void HAL_ClearDMASensors(HAL_DMAHandle handle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_STATE;
    return;
  }

  bool existingExternal = dma->aDMA->readConfig_ExternalClock(status);
  if (*status != 0) {
    return;
  }

  tDMA::tConfig config;
  std::memset(&config, 0, sizeof(config));
  config.Pause = true;
  config.ExternalClock = existingExternal;
  dma->aDMA->writeConfig(config, status);
}

void HAL_ClearDMAExternalTriggers(HAL_DMAHandle handle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_STATE;
    return;
  }

  dma->captureStore.triggerChannels = 0;
  tDMA::tExternalTriggers newTrigger;
  std::memset(&newTrigger, 0, sizeof(newTrigger));
  for (unsigned char reg = 0; reg < tDMA::kNumExternalTriggersRegisters;
       reg++) {
    for (unsigned char bit = 0; bit < tDMA::kNumExternalTriggersElements;
         bit++) {
      dma->aDMA->writeExternalTriggers(reg, bit, newTrigger, status);
    }
  }
}

void HAL_StartDMA(HAL_DMAHandle handle, int32_t queueDepth, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = HAL_INVALID_DMA_STATE;
    return;
  }

  tDMA::tConfig config = dma->aDMA->readConfig(status);
  if (*status != 0) {
    return;
  }

  {
    size_t accum_size = 0;
#define SET_SIZE(bit)                                      \
  if (config.bit) {                                        \
    dma->captureStore.channelOffsets[k##bit] = accum_size; \
    accum_size += kChannelSize[k##bit];                    \
  } else {                                                 \
    dma->captureStore.channelOffsets[k##bit] = -1;         \
  }
    SET_SIZE(Enable_AI0_Low);
    SET_SIZE(Enable_AI0_High);
    SET_SIZE(Enable_AIAveraged0_Low);
    SET_SIZE(Enable_AIAveraged0_High);
    SET_SIZE(Enable_AI1_Low);
    SET_SIZE(Enable_AI1_High);
    SET_SIZE(Enable_AIAveraged1_Low);
    SET_SIZE(Enable_AIAveraged1_High);
    SET_SIZE(Enable_Accumulator0);
    SET_SIZE(Enable_Accumulator1);
    SET_SIZE(Enable_DI);
    SET_SIZE(Enable_AnalogTriggers);
    SET_SIZE(Enable_Counters_Low);
    SET_SIZE(Enable_Counters_High);
    SET_SIZE(Enable_CounterTimers_Low);
    SET_SIZE(Enable_CounterTimers_High);
    SET_SIZE(Enable_Encoders_Low);
    SET_SIZE(Enable_Encoders_High);
    SET_SIZE(Enable_EncoderTimers_Low);
    SET_SIZE(Enable_EncoderTimers_High);
    SET_SIZE(Enable_DutyCycle_Low);
    SET_SIZE(Enable_DutyCycle_High);
#undef SET_SIZE
    dma->captureStore.captureSize = accum_size + 1;
  }

  uint32_t byteDepth = queueDepth * dma->captureStore.captureSize;

  dma->manager = std::make_unique<tDMAManager>(g_DMA_index, byteDepth, status);
  if (*status != 0) {
    return;
  }

  dma->aDMA->writeConfig_Pause(false, status);

  dma->manager->start(status);
  dma->manager->stop(status);
  dma->manager->start(status);
}

void HAL_StopDMA(HAL_DMAHandle handle, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    dma->aDMA->writeConfig_Pause(true, status);
    *status = 0;
    dma->manager->stop(status);
    dma->manager = nullptr;
  }
}

void* HAL_GetDMADirectPointer(HAL_DMAHandle handle) {
  auto dma = dmaHandles->Get(handle);
  return dma.get();
}

enum HAL_DMAReadStatus HAL_ReadDMADirect(void* dmaPointer,
                                         HAL_DMASample* dmaSample,
                                         double timeoutSeconds,
                                         int32_t* remainingOut,
                                         int32_t* status) {
  DMA* dma = static_cast<DMA*>(dmaPointer);
  *remainingOut = 0;
  size_t remainingBytes = 0;

  if (!dma->manager) {
    *status = HAL_INVALID_DMA_STATE;
    return HAL_DMA_ERROR;
  }

  dma->manager->read(dmaSample->readBuffer, dma->captureStore.captureSize,
                     static_cast<uint32_t>(timeoutSeconds * 1000),
                     &remainingBytes, status);

  *remainingOut = remainingBytes / dma->captureStore.captureSize;

  if (*status == 0) {
    uint32_t lower_sample =
        dmaSample->readBuffer[dma->captureStore.captureSize - 1];
    dmaSample->timeStamp = HAL_ExpandFPGATime(lower_sample, status);
    if (*status != 0) {
      return HAL_DMA_ERROR;
    }
    dmaSample->triggerChannels = dma->captureStore.triggerChannels;
    dmaSample->captureSize = dma->captureStore.captureSize;
    std::memcpy(dmaSample->channelOffsets, dma->captureStore.channelOffsets,
                sizeof(dmaSample->channelOffsets));
    return HAL_DMA_OK;
  } else if (*status == NiFpga_Status_FifoTimeout) {
    *status = 0;
    return HAL_DMA_TIMEOUT;
  } else {
    return HAL_DMA_ERROR;
  }
}

enum HAL_DMAReadStatus HAL_ReadDMA(HAL_DMAHandle handle,
                                   HAL_DMASample* dmaSample,
                                   double timeoutSeconds, int32_t* remainingOut,
                                   int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return HAL_DMA_ERROR;
  }

  return HAL_ReadDMADirect(dma.get(), dmaSample, timeoutSeconds, remainingOut,
                           status);
}

static uint32_t ReadDMAValue(const HAL_DMASample& dma, int valueType, int index,
                             int32_t* status) {
  auto offset = dma.channelOffsets[valueType];
  if (offset == -1) {
    *status = NiFpga_Status_ResourceNotFound;
    return 0;
  }
  return dma.readBuffer[offset + index];
}

uint64_t HAL_GetDMASampleTime(const HAL_DMASample* dmaSample, int32_t* status) {
  return dmaSample->timeStamp;
}

int32_t HAL_GetDMASampleEncoderRaw(const HAL_DMASample* dmaSample,
                                   HAL_EncoderHandle encoderHandle,
                                   int32_t* status) {
  HAL_FPGAEncoderHandle fpgaEncoderHandle = 0;
  HAL_CounterHandle counterHandle = 0;
  bool validEncoderHandle = hal::GetEncoderBaseHandle(
      encoderHandle, &fpgaEncoderHandle, &counterHandle);

  if (!validEncoderHandle) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  if (counterHandle != HAL_kInvalidHandle) {
    return HAL_GetDMASampleCounter(dmaSample, counterHandle, status);
  }

  if (getHandleType(fpgaEncoderHandle) != HAL_HandleEnum::FPGAEncoder) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int32_t index = getHandleIndex(fpgaEncoderHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  uint32_t dmaWord = 0;
  *status = 0;
  if (index < 4) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_Encoders_Low, index, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_Encoders_High, index - 4, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return -1;
  }

  return static_cast<int32_t>(dmaWord) >> 1;
}

int32_t HAL_GetDMASampleEncoderPeriodRaw(const HAL_DMASample* dmaSample,
                                         HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  HAL_FPGAEncoderHandle fpgaEncoderHandle = 0;
  HAL_CounterHandle counterHandle = 0;
  bool validEncoderHandle = hal::GetEncoderBaseHandle(
      encoderHandle, &fpgaEncoderHandle, &counterHandle);

  if (!validEncoderHandle) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  if (counterHandle != HAL_kInvalidHandle) {
    return HAL_GetDMASampleCounterPeriod(dmaSample, counterHandle, status);
  }

  if (getHandleType(fpgaEncoderHandle) != HAL_HandleEnum::FPGAEncoder) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int32_t index = getHandleIndex(fpgaEncoderHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  uint32_t dmaWord = 0;
  *status = 0;
  if (index < 4) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_EncoderTimers_Low, index, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_EncoderTimers_High, index - 4, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return -1;
  }

  return static_cast<int32_t>(dmaWord) & 0x7FFFFF;
}

int32_t HAL_GetDMASampleCounter(const HAL_DMASample* dmaSample,
                                HAL_CounterHandle counterHandle,
                                int32_t* status) {
  if (getHandleType(counterHandle) != HAL_HandleEnum::Counter) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int32_t index = getHandleIndex(counterHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  uint32_t dmaWord = 0;
  *status = 0;
  if (index < 4) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_Counters_Low, index, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_Counters_High, index - 4, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return -1;
  }

  return static_cast<int32_t>(dmaWord) >> 1;
}

int32_t HAL_GetDMASampleCounterPeriod(const HAL_DMASample* dmaSample,
                                      HAL_CounterHandle counterHandle,
                                      int32_t* status) {
  if (getHandleType(counterHandle) != HAL_HandleEnum::Counter) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int32_t index = getHandleIndex(counterHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  uint32_t dmaWord = 0;
  *status = 0;
  if (index < 4) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_CounterTimers_Low, index, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_CounterTimers_High, index - 4, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return -1;
  }

  return static_cast<int32_t>(dmaWord) & 0x7FFFFF;
}

HAL_Bool HAL_GetDMASampleDigitalSource(const HAL_DMASample* dmaSample,
                                       HAL_Handle dSourceHandle,
                                       int32_t* status) {
  HAL_HandleEnum handleType = getHandleType(dSourceHandle);
  int32_t index = getHandleIndex(dSourceHandle);

  *status = 0;
  if (handleType == HAL_HandleEnum::DIO) {
    auto readVal = ReadDMAValue(*dmaSample, kEnable_DI, 0, status);
    if (*status == 0) {
      if (index < kNumDigitalHeaders) {
        return (readVal >> index) & 0x1;
      } else {
        return (readVal >> (index + 6)) & 0x1;
      }
    }
  } else if (handleType == HAL_HandleEnum::AnalogTrigger) {
    auto readVal = ReadDMAValue(*dmaSample, kEnable_AnalogTriggers, 0, status);
    if (*status == 0) {
      return (readVal >> index) & 0x1;
    }
  } else {
    *status = NiFpga_Status_InvalidParameter;
  }
  return false;
}
int32_t HAL_GetDMASampleAnalogInputRaw(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int32_t* status) {
  if (getHandleType(aInHandle) != HAL_HandleEnum::AnalogInput) {
    *status = HAL_HANDLE_ERROR;
    return 0xFFFFFFFF;
  }

  int32_t index = getHandleIndex(aInHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return 0xFFFFFFFF;
  }

  uint32_t dmaWord = 0;
  if (index < 4) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_AI0_Low, index / 2, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_AI0_High, (index - 4) / 2, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return 0xFFFFFFFF;
  }

  if (index % 2) {
    return (dmaWord >> 16) & 0xffff;
  } else {
    return dmaWord & 0xffff;
  }
}

int32_t HAL_GetDMASampleAveragedAnalogInputRaw(const HAL_DMASample* dmaSample,
                                               HAL_AnalogInputHandle aInHandle,
                                               int32_t* status) {
  if (getHandleType(aInHandle) != HAL_HandleEnum::AnalogInput) {
    *status = HAL_HANDLE_ERROR;
    return 0xFFFFFFFF;
  }

  int32_t index = getHandleIndex(aInHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return 0xFFFFFFFF;
  }

  uint32_t dmaWord = 0;
  if (index < 4) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_AIAveraged0_Low, index, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_AIAveraged0_High, index - 4, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return 0xFFFFFFFF;
  }

  return dmaWord;
}

void HAL_GetDMASampleAnalogAccumulator(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int64_t* count, int64_t* value,
                                       int32_t* status) {
  if (!HAL_IsAccumulatorChannel(aInHandle, status)) {
    *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
    return;
  }

  int32_t index = getHandleIndex(aInHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t dmaWord = 0;
  uint32_t dmaValue1 = 0;
  uint32_t dmaValue2 = 0;
  if (index == 0) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_Accumulator0, index, status);
    dmaValue1 =
        ReadDMAValue(*dmaSample, kEnable_Accumulator0, index + 1, status);
    dmaValue2 =
        ReadDMAValue(*dmaSample, kEnable_Accumulator0, index + 2, status);
  } else if (index == 1) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_Accumulator1, index - 1, status);
    dmaValue1 = ReadDMAValue(*dmaSample, kEnable_Accumulator0, index, status);
    dmaValue2 =
        ReadDMAValue(*dmaSample, kEnable_Accumulator0, index + 1, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return;
  }

  *count = dmaWord;

  *value = static_cast<int64_t>(dmaValue1) << 32 | dmaValue2;
}

int32_t HAL_GetDMASampleDutyCycleOutputRaw(const HAL_DMASample* dmaSample,
                                           HAL_DutyCycleHandle dutyCycleHandle,
                                           int32_t* status) {
  if (getHandleType(dutyCycleHandle) != HAL_HandleEnum::DutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int32_t index = getHandleIndex(dutyCycleHandle);
  if (index < 0) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  uint32_t dmaWord = 0;
  *status = 0;
  if (index < 4) {
    dmaWord = ReadDMAValue(*dmaSample, kEnable_DutyCycle_Low, index, status);
  } else if (index < 8) {
    dmaWord =
        ReadDMAValue(*dmaSample, kEnable_DutyCycle_High, index - 4, status);
  } else {
    *status = NiFpga_Status_ResourceNotFound;
  }
  if (*status != 0) {
    return -1;
  }
  return dmaWord;
}
}  // extern "C"
