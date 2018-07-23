/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInternal.h"

#include <atomic>

#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/AnalogInput.h"
#include "hal/ChipObject.h"

namespace hal {

wpi::mutex analogRegisterWindowMutex;
std::unique_ptr<tAI> analogInputSystem;
std::unique_ptr<tAO> analogOutputSystem;

IndexedHandleResource<HAL_AnalogInputHandle, ::hal::AnalogPort,
                      kNumAnalogInputs, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;

static int32_t analogNumChannelsToActivate = 0;

static std::atomic<bool> analogSystemInitialized{false};

bool analogSampleRateSet = false;

namespace init {
void InitializeAnalogInternal() {
  static IndexedHandleResource<HAL_AnalogInputHandle, ::hal::AnalogPort,
                               kNumAnalogInputs, HAL_HandleEnum::AnalogInput>
      alH;
  analogInputHandles = &alH;
}
}  // namespace init

void initializeAnalog(int32_t* status) {
  hal::init::CheckInit();
  if (analogSystemInitialized) return;
  std::lock_guard<wpi::mutex> lock(analogRegisterWindowMutex);
  if (analogSystemInitialized) return;
  analogInputSystem.reset(tAI::create(status));
  analogOutputSystem.reset(tAO::create(status));
  setAnalogNumChannelsToActivate(kNumAnalogInputs);
  setAnalogSampleRate(kDefaultSampleRate, status);
  analogSystemInitialized = true;
}

int32_t getAnalogNumActiveChannels(int32_t* status) {
  int32_t scanSize = analogInputSystem->readConfig_ScanSize(status);
  if (scanSize == 0) return 8;
  return scanSize;
}

void setAnalogNumChannelsToActivate(int32_t channels) {
  analogNumChannelsToActivate = channels;
}

int32_t getAnalogNumChannelsToActivate(int32_t* status) {
  if (analogNumChannelsToActivate == 0)
    return getAnalogNumActiveChannels(status);
  return analogNumChannelsToActivate;
}

void setAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  // TODO: This will change when variable size scan lists are implemented.
  // TODO: Need double comparison with epsilon.
  // wpi_assert(!sampleRateSet || GetSampleRate() == samplesPerSecond);
  analogSampleRateSet = true;

  // Compute the convert rate
  uint32_t ticksPerSample =
      static_cast<uint32_t>(static_cast<double>(kTimebase) / samplesPerSecond);
  uint32_t ticksPerConversion =
      ticksPerSample / getAnalogNumChannelsToActivate(status);
  // ticksPerConversion must be at least 80
  if (ticksPerConversion < 80) {
    if ((*status) >= 0) *status = SAMPLE_RATE_TOO_HIGH;
    ticksPerConversion = 80;
  }

  // Atomically set the scan size and the convert rate so that the sample rate
  // is constant
  tAI::tConfig config;
  config.ScanSize = getAnalogNumChannelsToActivate(status);
  config.ConvertRate = ticksPerConversion;
  analogInputSystem->writeConfig(config, status);

  // Indicate that the scan size has been commited to hardware.
  setAnalogNumChannelsToActivate(0);
}

}  // namespace hal
