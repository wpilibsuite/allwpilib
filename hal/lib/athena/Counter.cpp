/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Counter.h"

#include "DigitalInternal.h"
#include "HAL/HAL.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");

using namespace hal;

extern "C" {
struct counter_t {
  tCounter* counter;
  uint32_t index;
};
typedef struct counter_t Counter;

static hal::Resource* counters = nullptr;

void* initializeCounter(Mode mode, uint32_t* index, int32_t* status) {
  hal::Resource::CreateResourceObject(&counters, tCounter::kNumSystems);
  *index = counters->Allocate("Counter");
  if (*index == ~0ul) {
    *status = NO_AVAILABLE_RESOURCES;
    return nullptr;
  }
  Counter* counter = new Counter();
  counter->counter = tCounter::create(*index, status);
  counter->counter->writeConfig_Mode(mode, status);
  counter->counter->writeTimerConfig_AverageSize(1, status);
  counter->index = *index;
  return counter;
}

void freeCounter(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  if (!counter) return;
  delete counter->counter;
  counters->Free(counter->index);
}

void setCounterAverageSize(void* counter_pointer, int32_t size,
                           int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeTimerConfig_AverageSize(size, status);
}

/**
 * Set the source object that causes the counter to count up.
 * Set the up counting DigitalSource.
 */
void setCounterUpSource(void* counter_pointer, uint32_t pin, bool analogTrigger,
                        int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;

  uint8_t module;

  remapDigitalSource(analogTrigger, pin, module);

  counter->counter->writeConfig_UpSource_Module(module, status);
  counter->counter->writeConfig_UpSource_Channel(pin, status);
  counter->counter->writeConfig_UpSource_AnalogTrigger(analogTrigger, status);

  if (counter->counter->readConfig_Mode(status) == kTwoPulse ||
      counter->counter->readConfig_Mode(status) == kExternalDirection) {
    setCounterUpSourceEdge(counter_pointer, true, false, status);
  }
  counter->counter->strobeReset(status);
}

/**
 * Set the edge sensitivity on an up counting source.
 * Set the up source to either detect rising edges or falling edges.
 */
void setCounterUpSourceEdge(void* counter_pointer, bool risingEdge,
                            bool fallingEdge, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeConfig_UpRisingEdge(risingEdge, status);
  counter->counter->writeConfig_UpFallingEdge(fallingEdge, status);
}

/**
 * Disable the up counting source to the counter.
 */
void clearCounterUpSource(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
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
void setCounterDownSource(void* counter_pointer, uint32_t pin,
                          bool analogTrigger, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  unsigned char mode = counter->counter->readConfig_Mode(status);
  if (mode != kTwoPulse && mode != kExternalDirection) {
    // TODO: wpi_setWPIErrorWithContext(ParameterOutOfRange, "Counter only
    // supports DownSource in TwoPulse and ExternalDirection modes.");
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  uint8_t module;

  remapDigitalSource(analogTrigger, pin, module);

  counter->counter->writeConfig_DownSource_Module(module, status);
  counter->counter->writeConfig_DownSource_Channel(pin, status);
  counter->counter->writeConfig_DownSource_AnalogTrigger(analogTrigger, status);

  setCounterDownSourceEdge(counter_pointer, true, false, status);
  counter->counter->strobeReset(status);
}

/**
 * Set the edge sensitivity on a down counting source.
 * Set the down source to either detect rising edges or falling edges.
 */
void setCounterDownSourceEdge(void* counter_pointer, bool risingEdge,
                              bool fallingEdge, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeConfig_DownRisingEdge(risingEdge, status);
  counter->counter->writeConfig_DownFallingEdge(fallingEdge, status);
}

/**
 * Disable the down counting source to the counter.
 */
void clearCounterDownSource(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
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
void setCounterUpDownMode(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeConfig_Mode(kTwoPulse, status);
}

/**
 * Set external direction mode on this counter.
 * Counts are sourced on the Up counter input.
 * The Down counter input represents the direction to count.
 */
void setCounterExternalDirectionMode(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeConfig_Mode(kExternalDirection, status);
}

/**
 * Set Semi-period mode on this counter.
 * Counts up on both rising and falling edges.
 */
void setCounterSemiPeriodMode(void* counter_pointer, bool highSemiPeriod,
                              int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeConfig_Mode(kSemiperiod, status);
  counter->counter->writeConfig_UpRisingEdge(highSemiPeriod, status);
  setCounterUpdateWhenEmpty(counter_pointer, false, status);
}

/**
 * Configure the counter to count in up or down based on the length of the input
 * pulse.
 * This mode is most useful for direction sensitive gear tooth sensors.
 * @param threshold The pulse length beyond which the counter counts the
 * opposite direction.  Units are seconds.
 */
void setCounterPulseLengthMode(void* counter_pointer, double threshold,
                               int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeConfig_Mode(kPulseLength, status);
  counter->counter->writeConfig_PulseLengthThreshold(
      (uint32_t)(threshold * 1.0e6) * kSystemClockTicksPerMicrosecond, status);
}

/**
 * Get the Samples to Average which specifies the number of samples of the timer
 * to
 * average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
int32_t getCounterSamplesToAverage(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  return counter->counter->readTimerConfig_AverageSize(status);
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void setCounterSamplesToAverage(void* counter_pointer, int samplesToAverage,
                                int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
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
void resetCounter(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->strobeReset(status);
}

/**
 * Read the current counter value.
 * Read the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 */
int32_t getCounter(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  int32_t value = counter->counter->readOutput_Value(status);
  return value;
}

/*
 * Get the Period of the most recent count.
 * Returns the time interval of the most recent count. This can be used for
 * velocity calculations to determine shaft speed.
 * @returns The period of the last two pulses in units of seconds.
 */
double getCounterPeriod(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  tCounter::tTimerOutput output = counter->counter->readTimerOutput(status);
  double period;
  if (output.Stalled) {
    // Return infinity
    double zero = 0.0;
    period = 1.0 / zero;
  } else {
    // output.Period is a fixed point number that counts by 2 (24 bits, 25
    // integer bits)
    period = (double)(output.Period << 1) / (double)output.Count;
  }
  return period * 2.5e-8;  // result * timebase (currently 40ns)
}

/**
 * Set the maximum period where the device is still considered "moving".
 * Sets the maximum period where the device is considered moving. This value is
 * used to determine the "stopped" state of the counter using the GetStopped
 * method.
 * @param maxPeriod The maximum period where the counted device is considered
 * moving in seconds.
 */
void setCounterMaxPeriod(void* counter_pointer, double maxPeriod,
                         int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  counter->counter->writeTimerConfig_StallPeriod((uint32_t)(maxPeriod * 4.0e8),
                                                 status);
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
void setCounterUpdateWhenEmpty(void* counter_pointer, bool enabled,
                               int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
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
bool getCounterStopped(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  return counter->counter->readTimerOutput_Stalled(status);
}

/**
 * The last direction the counter value changed.
 * @return The last direction the counter value changed.
 */
bool getCounterDirection(void* counter_pointer, int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  bool value = counter->counter->readOutput_Direction(status);
  return value;
}

/**
 * Set the Counter to return reversed sensing on the direction.
 * This allows counters to change the direction they are counting in the case of
 * 1X and 2X quadrature encoding only. Any other counter mode isn't supported.
 * @param reverseDirection true if the value counted should be negated.
 */
void setCounterReverseDirection(void* counter_pointer, bool reverseDirection,
                                int32_t* status) {
  Counter* counter = (Counter*)counter_pointer;
  if (counter->counter->readConfig_Mode(status) == kExternalDirection) {
    if (reverseDirection)
      setCounterDownSourceEdge(counter_pointer, true, true, status);
    else
      setCounterDownSourceEdge(counter_pointer, false, true, status);
  }
}
}
