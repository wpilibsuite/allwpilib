#pragma once
#include <stdint.h>
#include "HAL/Port.h"
#include "ChipObject.h"
#include "HAL/cpp/Resource.hpp"
#include "HAL/cpp/priority_mutex.h"

static const uint32_t kExpectedLoopTiming = 40;
static const uint32_t kDigitalPins = 26;
static const uint32_t kPwmPins = 20;
static const uint32_t kRelayPins = 8;
static const uint32_t kNumHeaders = 10; // Number of non-MXP pins

/**
 * kDefaultPwmPeriod is in ms
 *
 * - 20ms periods (50 Hz) are the "safest" setting in that this works for all devices
 * - 20ms periods seem to be desirable for Vex Motors
 * - 20ms periods are the specified period for HS-322HD servos, but work reliably down
 *      to 10.0 ms; starting at about 8.5ms, the servo sometimes hums and get hot;
 *      by 5.0ms the hum is nearly continuous
 * - 10ms periods work well for Victor 884
 * - 5ms periods allows higher update rates for Luminary Micro Jaguar speed controllers.
 *      Due to the shipping firmware on the Jaguar, we can't run the update period less
 *      than 5.05 ms.
 *
 * kDefaultPwmPeriod is the 1x period (5.05 ms).  In hardware, the period scaling is implemented as an
 * output squelch to get longer periods for old devices.
 */
static const float kDefaultPwmPeriod = 5.05;
/**
 * kDefaultPwmCenter is the PWM range center in ms
 */
static const float kDefaultPwmCenter = 1.5;
/**
 * kDefaultPWMStepsDown is the number of PWM steps below the centerpoint
 */
static const int32_t kDefaultPwmStepsDown = 1000;
static const int32_t kPwmDisabled = 0;

struct PWMPort {
  Port port;
  int32_t maxPwm;
  int32_t deadbandMaxPwm;
  int32_t centerPwm;
  int32_t deadbandMinPwm;
  int32_t minPwm;
  bool eliminateDeadband;
};

// Create a mutex to protect changes to the digital output values
extern priority_recursive_mutex digitalDIOMutex;
// Create a mutex to protect changes to the relay values
extern priority_recursive_mutex digitalRelayMutex;
// Create a mutex to protect changes to the DO PWM config
extern priority_recursive_mutex digitalPwmMutex;
extern priority_recursive_mutex digitalI2COnBoardMutex;
extern priority_recursive_mutex digitalI2CMXPMutex;

extern tDIO* digitalSystem;
extern tRelay* relaySystem;
extern tPWM* pwmSystem;
extern hal::Resource *DIOChannels;
extern hal::Resource *DO_PWMGenerators;
extern hal::Resource *PWMChannels;

extern "C"
{
void initializeDigital(int32_t *status);
}