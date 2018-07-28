/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInternal.h"

#include <atomic>
#include <thread>

#include <FRC_NetworkCommunication/LoadOut.h>
#include <wpi/mutex.h>

#include "ConstantsInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/AnalogTrigger.h"
#include "hal/ChipObject.h"
#include "hal/HAL.h"
#include "hal/Ports.h"
#include "hal/cpp/UnsafeDIO.h"

namespace hal {

std::unique_ptr<tDIO> digitalSystem;
std::unique_ptr<tRelay> relaySystem;
std::unique_ptr<tPWM> pwmSystem;
std::unique_ptr<tSPI> spiSystem;

// Create a mutex to protect changes to the digital output values
wpi::mutex digitalDIOMutex;

DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                      kNumDigitalChannels + kNumPWMHeaders>*
    digitalChannelHandles;

namespace init {
void InitializeDigitalInternal() {
  static DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                               kNumDigitalChannels + kNumPWMHeaders>
      dcH;
  digitalChannelHandles = &dcH;
}
}  // namespace init

namespace detail {
wpi::mutex& UnsafeGetDIOMutex() { return digitalDIOMutex; }
tDIO* UnsafeGetDigialSystem() { return digitalSystem.get(); }
int32_t ComputeDigitalMask(HAL_DigitalHandle handle, int32_t* status) {
  auto port = digitalChannelHandles->Get(handle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  tDIO::tDO output;
  output.value = 0;
  if (port->channel >= kNumDigitalHeaders + kNumDigitalMXPChannels) {
    output.SPIPort = (1u << remapSPIChannel(port->channel));
  } else if (port->channel < kNumDigitalHeaders) {
    output.Headers = (1u << port->channel);
  } else {
    output.MXP = (1u << remapMXPChannel(port->channel));
  }
  return output.value;
}
}  // namespace detail

void initializeDigital(int32_t* status) {
  hal::init::CheckInit();
  static std::atomic_bool initialized{false};
  static wpi::mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return;

  std::lock_guard<wpi::mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return;

  digitalSystem.reset(tDIO::create(status));

  // Relay Setup
  relaySystem.reset(tRelay::create(status));

  // Turn off all relay outputs.
  relaySystem->writeValue_Forward(0, status);
  relaySystem->writeValue_Reverse(0, status);

  // PWM Setup
  pwmSystem.reset(tPWM::create(status));

  // Make sure that the 9403 IONode has had a chance to initialize before
  // continuing.
  while (pwmSystem->readLoopTiming(status) == 0) std::this_thread::yield();

  if (pwmSystem->readLoopTiming(status) != kExpectedLoopTiming) {
    *status = LOOP_TIMING_ERROR;  // NOTE: Doesn't display the error
  }

  // Calculate the length, in ms, of one DIO loop
  double loopTime = pwmSystem->readLoopTiming(status) /
                    (kSystemClockTicksPerMicrosecond * 1e3);

  pwmSystem->writeConfig_Period(
      static_cast<uint16_t>(kDefaultPwmPeriod / loopTime + .5), status);
  uint16_t minHigh = static_cast<uint16_t>(
      (kDefaultPwmCenter - kDefaultPwmStepsDown * loopTime) / loopTime + .5);
  pwmSystem->writeConfig_MinHigh(minHigh, status);
  // Ensure that PWM output values are set to OFF
  for (uint8_t pwmIndex = 0; pwmIndex < kNumPWMChannels; pwmIndex++) {
    // Copy of SetPWM
    if (pwmIndex < tPWM::kNumHdrRegisters) {
      pwmSystem->writeHdr(pwmIndex, kPwmDisabled, status);
    } else {
      pwmSystem->writeMXP(pwmIndex - tPWM::kNumHdrRegisters, kPwmDisabled,
                          status);
    }

    // Copy of SetPWMPeriodScale, set to 4x by default.
    if (pwmIndex < tPWM::kNumPeriodScaleHdrElements) {
      pwmSystem->writePeriodScaleHdr(pwmIndex, 3, status);
    } else {
      pwmSystem->writePeriodScaleMXP(
          pwmIndex - tPWM::kNumPeriodScaleHdrElements, 3, status);
    }
  }

  // SPI setup
  spiSystem.reset(tSPI::create(status));

  initialized = true;
}

bool remapDigitalSource(HAL_Handle digitalSourceHandle,
                        HAL_AnalogTriggerType analogTriggerType,
                        uint8_t& channel, uint8_t& module,
                        bool& analogTrigger) {
  if (isHandleType(digitalSourceHandle, HAL_HandleEnum::AnalogTrigger)) {
    // If handle passed, index is not negative
    int32_t index = getHandleIndex(digitalSourceHandle);
    channel = (index << 2) + analogTriggerType;
    module = channel >> 4;
    analogTrigger = true;
    return true;
  } else if (isHandleType(digitalSourceHandle, HAL_HandleEnum::DIO)) {
    int32_t index = getHandleIndex(digitalSourceHandle);
    if (index > kNumDigitalHeaders + kNumDigitalMXPChannels) {
      // channels 10-15, so need to add headers to remap index
      channel = remapSPIChannel(index) + kNumDigitalHeaders;
      module = 0;
    } else if (index >= kNumDigitalHeaders) {
      channel = remapMXPChannel(index);
      module = 1;
    } else {
      channel = index;
      module = 0;
    }
    analogTrigger = false;
    return true;
  } else {
    return false;
  }
}

int32_t remapMXPChannel(int32_t channel) { return channel - 10; }

int32_t remapMXPPWMChannel(int32_t channel) {
  if (channel < 14) {
    return channel - 10;  // first block of 4 pwms (MXP 0-3)
  } else {
    return channel - 6;  // block of PWMs after SPI
  }
}

int32_t remapSPIChannel(int32_t channel) { return channel - 26; }

}  // namespace hal

// Unused function here to test template compile.
__attribute__((unused)) static void CompileFunctorTest() {
  hal::UnsafeManipulateDIO(0, nullptr, [](hal::DIOSetProxy& proxy) {});
}
