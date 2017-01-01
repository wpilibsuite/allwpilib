/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Counter.h"

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HAL/HAL.h"
#include "HAL/handles/LimitedHandleResource.h"
#include "PortsInternal.h"

using namespace hal;

namespace {
struct Counter {
  std::unique_ptr<tCounter> counter;
  uint8_t index;
};
}

static LimitedHandleResource<HAL_CounterHandle, Counter, kNumCounters,
                             HAL_HandleEnum::Counter>
    counterHandles;

extern "C" {
HAL_CounterHandle HAL_InitializeCounter(HAL_Counter_Mode mode, int32_t* index,
                                        int32_t* status) {
  auto handle = counterHandles.Allocate();
  if (handle == HAL_kInvalidHandle) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto counter = counterHandles.Get(handle);
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
  counterHandles.Free(counterHandle);
}

void HAL_SetCounterAverageSize(HAL_CounterHandle counterHandle, int32_t size,
                               int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeTimerConfig_AverageSize(size, status);
}

/**
 * Set the source object that causes the counter to count up.
 * Set the up counting DigitalSource.
 */
void HAL_SetCounterUpSource(HAL_CounterHandle counterHandle,
                            HAL_Handle digitalSourceHandle,
                            HAL_AnalogTriggerType analogTriggerType,
                            int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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

/**
 * Set the edge sensitivity on an up counting source.
 * Set the up source to either detect rising edges or falling edges.
 */
void HAL_SetCounterUpSourceEdge(HAL_CounterHandle counterHandle,
                                HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_UpRisingEdge(risingEdge, status);
  counter->counter->writeConfig_UpFallingEdge(fallingEdge, status);
}

/**
 * Disable the up counting source to the counter.
 */
void HAL_ClearCounterUpSource(HAL_CounterHandle counterHandle,
                              int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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

/**
 * Set the source object that causes the counter to count down.
 * Set the down counting DigitalSource.
 */
void HAL_SetCounterDownSource(HAL_CounterHandle counterHandle,
                              HAL_Handle digitalSourceHandle,
                              HAL_AnalogTriggerType analogTriggerType,
                              int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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

/**
 * Set the edge sensitivity on a down counting source.
 * Set the down source to either detect rising edges or falling edges.
 */
void HAL_SetCounterDownSourceEdge(HAL_CounterHandle counterHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_DownRisingEdge(risingEdge, status);
  counter->counter->writeConfig_DownFallingEdge(fallingEdge, status);
}

/**
 * Disable the down counting source to the counter.
 */
void HAL_ClearCounterDownSource(HAL_CounterHandle counterHandle,
                                int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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

/**
 * Set standard up / down counting mode on this counter.
 * Up and down counts are sourced independently from two inputs.
 */
void HAL_SetCounterUpDownMode(HAL_CounterHandle counterHandle,
                              int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kTwoPulse, status);
}

/**
 * Set external direction mode on this counter.
 * Counts are sourced on the Up counter input.
 * The Down counter input represents the direction to count.
 */
void HAL_SetCounterExternalDirectionMode(HAL_CounterHandle counterHandle,
                                         int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kExternalDirection, status);
}

/**
 * Set Semi-period mode on this counter.
 * Counts up on both rising and falling edges.
 */
void HAL_SetCounterSemiPeriodMode(HAL_CounterHandle counterHandle,
                                  HAL_Bool highSemiPeriod, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeConfig_Mode(HAL_Counter_kSemiperiod, status);
  counter->counter->writeConfig_UpRisingEdge(highSemiPeriod, status);
  HAL_SetCounterUpdateWhenEmpty(counterHandle, false, status);
}

/**
 * Configure the counter to count in up or down based on the length of the input
 * pulse.
 * This mode is most useful for direction sensitive gear tooth sensors.
 * @param threshold The pulse length beyond which the counter counts the
 * opposite direction.  Units are seconds.
 */
void HAL_SetCounterPulseLengthMode(HAL_CounterHandle counterHandle,
                                   double threshold, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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

/**
 * Get the Samples to Average which specifies the number of samples of the timer
 * to
 * average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
int32_t HAL_GetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                       int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return counter->counter->readTimerConfig_AverageSize(status);
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void HAL_SetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
  }
  counter->counter->writeTimerConfig_AverageSize(samplesToAverage, status);
}

/**
 * Reset the Counter to zero.
 * Set the counter value to zero. This doesn't effect the running state of the
 * counter, just sets the current value to zero.
 */
void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->strobeReset(status);
}

/**
 * Read the current counter value.
 * Read the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 */
int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t value = counter->counter->readOutput_Value(status);
  return value;
}

/*
 * Get the Period of the most recent count.
 * Returns the time interval of the most recent count. This can be used for
 * velocity calculations to determine shaft speed.
 * @returns The period of the last two pulses in units of seconds.
 */
double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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

/**
 * Set the maximum period where the device is still considered "moving".
 * Sets the maximum period where the device is considered moving. This value is
 * used to determine the "stopped" state of the counter using the GetStopped
 * method.
 * @param maxPeriod The maximum period where the counted device is considered
 * moving in seconds.
 */
void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeTimerConfig_StallPeriod(
      static_cast<uint32_t>(maxPeriod * 4.0e8), status);
}

/**
 * Select whether you want to continue updating the event timer output when
 * there are no samples captured. The output of the event timer has a buffer of
 * periods that are averaged and posted to a register on the FPGA.  When the
 * timer detects that the event source has stopped (based on the MaxPeriod) the
 * buffer of samples to be averaged is emptied.  If you enable the update when
 * empty, you will be notified of the stopped source and the event time will
 * report 0 samples.  If you disable update when empty, the most recent average
 * will remain on the output until a new sample is acquired.  You will never see
 * 0 samples output (except when there have been no events since an FPGA reset)
 * and you will likely not see the stopped bit become true (since it is updated
 * at the end of an average and there are no samples to average).
 */
void HAL_SetCounterUpdateWhenEmpty(HAL_CounterHandle counterHandle,
                                   HAL_Bool enabled, int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  counter->counter->writeTimerConfig_UpdateWhenEmpty(enabled, status);
}

/**
 * Determine if the clock is stopped.
 * Determine if the clocked input is stopped based on the MaxPeriod value set
 * using the SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the
 * device (and counter) are assumed to be stopped and it returns true.
 * @return Returns true if the most recent counter period exceeds the MaxPeriod
 * value set by SetMaxPeriod.
 */
HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return counter->counter->readTimerOutput_Stalled(status);
}

/**
 * The last direction the counter value changed.
 * @return The last direction the counter value changed.
 */
HAL_Bool HAL_GetCounterDirection(HAL_CounterHandle counterHandle,
                                 int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
  if (counter == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value = counter->counter->readOutput_Direction(status);
  return value;
}

/**
 * Set the Counter to return reversed sensing on the direction.
 * This allows counters to change the direction they are counting in the case of
 * 1X and 2X quadrature encoding only. Any other counter mode isn't supported.
 * @param reverseDirection true if the value counted should be negated.
 */
void HAL_SetCounterReverseDirection(HAL_CounterHandle counterHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  auto counter = counterHandles.Get(counterHandle);
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
}
