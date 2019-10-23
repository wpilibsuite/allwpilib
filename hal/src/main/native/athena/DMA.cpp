/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/DMA.h"

#include <array>
#include <cstddef>
#include <memory>

#include "EncoderInternal.h"
#include "PortsInternal.h"
#include "hal/AnalogInput.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

namespace {
struct DMACaptureStore {
  // The offsets into the sample structure for each DMA type, or -1 if it isn't
  // in the set of values.
  int32_t channel_offsets[20];

  std::array<bool, 8> trigger_channels = {false, false, false, false,
                                          false, false, false, false};
  size_t capture_size;
};

struct DMA {
  std::unique_ptr<tDMAManager> manager;
  std::unique_ptr<tDMA> aDMA;

  DMACaptureStore captureStore;
};

struct DMASample {
  uint32_t readBuffer[64];
  uint64_t timeStamp;
  DMACaptureStore captureStore;
};
}  // namespace

static constexpr size_t kChannelSize[20] = {
    2, 2, 4, 4, 2, 2, 4, 4, 3, 3, 2, 1, 4, 4, 4, 4, 4, 4, 4, 4,
};

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
};

static hal::UnlimitedHandleResource<HAL_DMASampleHandle, DMASample,
                                    HAL_HandleEnum::DMASample>*
    dmaSampleHandles;
static hal::LimitedHandleResource<HAL_DMAHandle, DMA, 1,
                                  HAL_HandleEnum::DMASample>* dmaHandles;

namespace hal {
namespace init {
void InitializeDMA() {
  static hal::UnlimitedHandleResource<HAL_DMASampleHandle, DMASample,
                                      HAL_HandleEnum::DMASample>
      dsH;
  static hal::LimitedHandleResource<HAL_DMAHandle, DMA, 1,
                                    HAL_HandleEnum::DMASample>
      dH;
  dmaSampleHandles = &dsH;
  dmaHandles = &dH;
}
}  // namespace init
}  // namespace hal

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

  dma->aDMA->writeConfig_ExternalClock(false, status);
  if (*status != 0) {
    dmaHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  HAL_SetDMARate(handle, 1, status);
  if (*status != 0) {
    dmaHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  HAL_SetDMAPause(handle, false, status);
  return handle;
}

void HAL_FreeDMA(HAL_DMAHandle handle) {
  auto dma = dmaHandles->Get(handle);
  dmaHandles->Free(handle);

  if (!dma) return;

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

  dma->aDMA->writeConfig_Pause(pause, status);
}
void HAL_SetDMARate(HAL_DMAHandle handle, int32_t cycles, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (cycles < 1) {
    cycles = 1;
  }

  dma->aDMA->writeRate(static_cast<uint32_t>(cycles), status);
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
void HAL_AddDMADigitalSource(HAL_DMAHandle handle,
                             HAL_Handle digitalSourceHandle, int32_t* status);
void HAL_AddDMAAnalogInput(HAL_DMAHandle handle,
                           HAL_AnalogInputHandle aInHandle, int32_t* status);

void HAL_SetDMAExternalTrigger(HAL_DMAHandle handle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_Bool rising, HAL_Bool falling,
                               int32_t* status);

void HAL_StartDMA(HAL_DMAHandle handle, int32_t queueDepth, int32_t* status) {
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (dma->manager) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  tDMA::tConfig config = dma->aDMA->readConfig(status);
  if (*status != 0) return;

  {
    size_t accum_size = 0;
#define SET_SIZE(bit)                                       \
  if (config.bit) {                                         \
    dma->captureStore.channel_offsets[k##bit] = accum_size; \
    accum_size += kChannelSize[k##bit];                     \
  } else {                                                  \
    dma->captureStore.channel_offsets[k##bit] = -1;         \
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
#undef SET_SIZE
    dma->captureStore.capture_size = accum_size + 1;
  }

  dma->manager = std::make_unique<tDMAManager>(
      1, queueDepth * dma->captureStore.capture_size, status);
  if (*status != 0) {
    return;
  }

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
    dma->manager->stop(status);
    dma->manager = nullptr;
  }
}

enum HAL_DMAReadStatus HAL_ReadDMA(HAL_DMAHandle handle,
                                   HAL_DMASampleHandle dmaSampleHandle,
                                   int32_t timeoutMs, int32_t* remainingOut,
                                   int32_t* status) {
  auto dmaSample = dmaSampleHandles->Get(dmaSampleHandle);
  if (!dmaSample) {
    *status = HAL_HANDLE_ERROR;
    return HAL_DMA_ERROR;
  }
  auto dma = dmaHandles->Get(handle);
  if (!dma) {
    *status = HAL_HANDLE_ERROR;
    return HAL_DMA_ERROR;
  }

  *remainingOut = 0;
  size_t remainingBytes = 0;

  if (!dma->manager) {
    *status = INCOMPATIBLE_STATE;
    return HAL_DMA_ERROR;
  }

  dma->manager->read(dmaSample->readBuffer, dma->captureStore.capture_size,
                     timeoutMs, &remainingBytes, status);

  *remainingOut = remainingBytes / dma->captureStore.capture_size;

  if (*status == 0) {
    uint32_t lower_sample =
        dmaSample->readBuffer[dma->captureStore.capture_size - 1];
    dmaSample->timeStamp = HAL_ExpandFPGATime(lower_sample, status);
    if (*status != 0) {
      return HAL_DMA_ERROR;
    }
    dmaSample->captureStore = dma->captureStore;
    return HAL_DMA_OK;
  } else if (*status == NiFpga_Status_FifoTimeout) {
    *status = 0;
    return HAL_DMA_TIMEOUT;
  } else {
    return HAL_DMA_ERROR;
  }
}

HAL_DMASampleHandle HAL_MakeDMASample(int32_t* status) {
  auto dmaSample = std::make_shared<DMASample>();
  auto sampleHandle = dmaSampleHandles->Allocate(dmaSample);
  if (sampleHandle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
  }
  return sampleHandle;
}

void HAL_FreeDMASample(HAL_DMASampleHandle handle) {
  dmaSampleHandles->Free(handle);
}

static uint32_t ReadDMAValue(const DMASample& dma, int valueType, int index,
                             int32_t* status) {
  auto offset = dma.captureStore.channel_offsets[valueType];
  if (offset == -1) {
    *status = NiFpga_Status_ResourceNotFound;
    return 0;
  }
  return dma.readBuffer[offset + index];
}

// Sampling Code
double HAL_GetDMASampleTimestamp(HAL_DMASampleHandle dmaSampleHandle,
                                 int32_t* status) {
  return static_cast<double>(HAL_GetDMASampleTime(dmaSampleHandle, status)) *
         0.000001;
}
uint64_t HAL_GetDMASampleTime(HAL_DMASampleHandle dmaSampleHandle,
                              int32_t* status) {
  auto dmaSample = dmaSampleHandles->Get(dmaSampleHandle);
  if (!dmaSample) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return dmaSample->timeStamp;
}

int32_t HAL_GetDMASampleEncoder(HAL_DMASampleHandle dmaSampleHandle,
                                HAL_EncoderHandle encoderHandle,
                                int32_t* status) {
  auto scale = HAL_GetEncoderEncodingScale(encoderHandle, status);
  if (*status != 0) {
    return 0;
  }

  return HAL_GetDMASampleEncoderRaw(dmaSampleHandle, encoderHandle, status) /
         scale;
}
int32_t HAL_GetDMASampleEncoderRaw(HAL_DMASampleHandle dmaSampleHandle,
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
    return HAL_GetDMASampleCounter(dmaSampleHandle, counterHandle, status);
  }

  auto dmaSample = dmaSampleHandles->Get(dmaSampleHandle);
  if (!dmaSample) {
    *status = HAL_HANDLE_ERROR;
    return -1;
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
  int32_t result = dmaWord;
  result = result << 1;
  return result;
}
int32_t HAL_GetDMASampleCounter(HAL_DMASampleHandle dmaSampleHandle,
                                HAL_CounterHandle counterHandle,
                                int32_t* status) {
  auto dmaSample = dmaSampleHandles->Get(dmaSampleHandle);
  if (!dmaSample) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

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
  int32_t result = dmaWord;
  result = result << 1;
  return result;
}
HAL_Bool HAL_GetDMASampleDigitalSource(HAL_DMASampleHandle dmaSampleHandle,
                                       HAL_Handle dSourceHandle,
                                       int32_t* status) {
  auto dmaSample = dmaSampleHandles->Get(dmaSampleHandle);
  if (!dmaSample) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

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
int32_t HAL_GetDMASampleAnalogInputRaw(HAL_DMASampleHandle dmaSampleHandle,
                                       HAL_AnalogInputHandle aInHandle,
                                       int32_t* status) {
  auto dmaSample = dmaSampleHandles->Get(dmaSampleHandle);
  if (!dmaSample) {
    *status = HAL_HANDLE_ERROR;
    return 0xFFFFFFFF;
  }

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
double HAL_GetDMASampleAnalogInputVoltage(HAL_DMASampleHandle dmaSampleHandle,
                                          HAL_AnalogInputHandle aInHandle,
                                          int32_t* status) {
  auto value =
      HAL_GetDMASampleAnalogInputRaw(dmaSampleHandle, aInHandle, status);
  if (*status != 0) {
    return 0.0;
  }
  return HAL_GetAnalogValueToVolts(aInHandle, value, status);
}
}  // extern "C"
