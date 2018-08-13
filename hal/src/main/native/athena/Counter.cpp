/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Counter.h"

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/HAL.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace {

struct Counter {
  std::unique_ptr<tCounter> counter;
  uint8_t index;
};

}  // namespace

static LimitedHandleResource<HAL_CounterHandle, Counter, kNumCounters,
                             HAL_HandleEnum::Counter>* counterHandles;

namespace hal {
namespace init {
void InitializeCounter() {
  static LimitedHandleResource<HAL_CounterHandle, Counter, kNumCounters,
                               HAL_HandleEnum::Counter>
      ch;
  counterHandles = &ch;
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_CounterHandle HAL_InitializeCounter(HAL_Counter_Mode mode, int32_t* index,
                                        int32_t* status) {
  hal::init::CheckInit();
  auto handle = counterHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto counter = counterHandles->Get(handle);
  if (counter == nullptr) {  // would only occur on thread issues
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  counter->index = static_cast<uint8_t>(getHandleIndex(handle));
  *index = counter->index;

  counter->counter.reset(tCounter::create(counter->index, status));
  counter->counter->writeConfig_Mode(mode, status);
  counter->counter->writeTimerConfig_AverageSize(1, status);
  return handle;
}

void HAL_FreeCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  counterHandles->Free(counterHandle);
}

void HAL_SetCounterAverageSize(HAL_CounterHandle counterHandle, int32_t size,
                               int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeTimerConfig_AverageSize(size, status);
}

void HAL_SetCounterUpSource(HAL_CounterHandle counterHandle,
                            HAL_Handle digitalSourceHandle,
                            HAL_AnalogTriggerType analogTriggerType,
                            int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, analogTriggerType, routingChannel,
                         routingModule, routingAnalogTrigger);
  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  counter->counter->writeConfig_UpSource_Module(routingModule, status);
  counter->counter->writeConfig_UpSource_Channel(routingChannel, status);
  counter->counter->writeConfig_UpSource_AnalogTrigger(routingAnalogTrigger,
                                                       status);

  if (counter->counter->readConfig_Mode(status) == HAL_Counter_kTwoPulse ||
      counter->counter->readConfig_Mode(status) ==
          HAL_Counter_kExternalDirection) {
    HAL_SetCounterUpSourceEdge(counterHandle, true, false, status);
  }
  counter->counter->strobeReset(status);
}

void HAL_SetCounterUpSourceEdge(HAL_CounterHandle counterHandle,
                                HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_UpRisingEdge(risingEdge, status);
  counter->counter->writeConfig_UpFallingEdge(fallingEdge, status);
}

void HAL_ClearCounterUpSource(HAL_CounterHandle counterHandle,
                              int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_UpFallingEdge(false, status);
  counter->counter->writeConfig_UpRisingEdge(false, status);
  // Index 0 of digital is always 0.
  counter->counter->writeConfig_UpSource_Channel(0, status);
  counter->counter->writeConfig_UpSource_AnalogTrigger(false, status);
}

void HAL_SetCounterDownSource(HAL_CounterHandle counterHandle,
                              HAL_Handle digitalSourceHandle,
                              HAL_AnalogTriggerType analogTriggerType,
                              int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  uint8_t mode = counter->counter->readConfig_Mode(status);
  if (mode != HAL_Counter_kTwoPulse && mode != HAL_Counter_kExternalDirection) {
    // TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "Counter only
    // supports DownSource in TwoPulse and ExternalDirection modes.");
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, analogTriggerType, routingChannel,
                         routingModule, routingAnalogTrigger);
  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  counter->counter->writeConfig_DownSource_Module(routingModule, status);
  counter->counter->writeConfig_DownSource_Channel(routingChannel, status);
  counter->counter->writeConfig_DownSource_AnalogTrigger(routingAnalogTrigger,
                                                         status);

  HAL_SetCounterDownSourceEdge(counterHandle, true, false, status);
  counter->counter->strobeReset(status);
}

void HAL_SetCounterDownSourceEdge(HAL_CounterHandle counterHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_DownRisingEdge(risingEdge, status);
  counter->counter->writeConfig_DownFallingEdge(fallingEdge, status);
}

void HAL_ClearCounterDownSource(HAL_CounterHandle counterHandle,
                                int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_DownFallingEdge(false, status);
  counter->counter->writeConfig_DownRisingEdge(false, status);
  // Index 0 of digital is always 0.
  counter->counter->writeConfig_DownSource_Channel(0, status);
  counter->counter->writeConfig_DownSource_AnalogTrigger(false, status);
}

void HAL_SetCounterUpDownMode(HAL_CounterHandle counterHandle,
                              int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kTwoPulse, status);
}

void HAL_SetCounterExternalDirectionMode(HAL_CounterHandle counterHandle,
                                         int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kExternalDirection, status);
}

void HAL_SetCounterSemiPeriodMode(HAL_CounterHandle counterHandle,
                                  HAL_Bool highSemiPeriod, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kSemiperiod, status);
  counter->counter->writeConfig_UpRisingEdge(highSemiPeriod, status);
  HAL_SetCounterUpdateWhenEmpty(counterHandle, false, status);
}

void HAL_SetCounterPulseLengthMode(HAL_CounterHandle counterHandle,
                                   double threshold, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kPulseLength, status);
  counter->counter->writeConfig_PulseLengthThreshold(
      static_cast<uint32_t>(threshold * 1.0e6) *
          kSystemClockTicksPerMicrosecond,
      status);
}

int32_t HAL_GetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                       int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return counter->counter->readTimerConfig_AverageSize(status);
}

void HAL_SetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
  }
  counter->counter->writeTimerConfig_AverageSize(samplesToAverage, status);
}

void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->strobeReset(status);
}

int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t value = counter->counter->readOutput_Value(status);
  return value;
}

double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }
  tCounter::tTimerOutput output = counter->counter->readTimerOutput(status);
  double period;
  if (output.Stalled) {
    // Return infinity
    double zero = 0.0;
    period = 1.0 / zero;
  } else {
    // output.Period is a fixed point number that counts by 2 (24 bits, 25
    // integer bits)
    period = static_cast<double>(output.Period << 1) /
             static_cast<double>(output.Count);
  }
  return static_cast<double>(period *
                             2.5e-8);  // result * timebase (currently 25ns)
}

void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeTimerConfig_StallPeriod(
      static_cast<uint32_t>(maxPeriod * 4.0e8), status);
}

void HAL_SetCounterUpdateWhenEmpty(HAL_CounterHandle counterHandle,
                                   HAL_Bool enabled, int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeTimerConfig_UpdateWhenEmpty(enabled, status);
}

HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return counter->counter->readTimerOutput_Stalled(status);
}

HAL_Bool HAL_GetCounterDirection(HAL_CounterHandle counterHandle,
                                 int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value = counter->counter->readOutput_Direction(status);
  return value;
}

void HAL_SetCounterReverseDirection(HAL_CounterHandle counterHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  auto counter = counterHandles->Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (counter->counter->readConfig_Mode(status) ==
      HAL_Counter_kExternalDirection) {
    if (reverseDirection)
      HAL_SetCounterDownSourceEdge(counterHandle, true, true, status);
    else
      HAL_SetCounterDownSourceEdge(counterHandle, false, true, status);
  }
}

}  // extern "C"
