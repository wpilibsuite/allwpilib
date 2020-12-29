// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Ports.h"

#include "PortsInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePorts() {}
}  // namespace init
}  // namespace hal

extern "C" {

int32_t HAL_GetNumAccumulators(void) {
  return kNumAccumulators;
}
int32_t HAL_GetNumAnalogTriggers(void) {
  return kNumAnalogTriggers;
}
int32_t HAL_GetNumAnalogInputs(void) {
  return kNumAnalogInputs;
}
int32_t HAL_GetNumAnalogOutputs(void) {
  return kNumAnalogOutputs;
}
int32_t HAL_GetNumCounters(void) {
  return kNumCounters;
}
int32_t HAL_GetNumDigitalHeaders(void) {
  return kNumDigitalHeaders;
}
int32_t HAL_GetNumPWMHeaders(void) {
  return kNumPWMHeaders;
}
int32_t HAL_GetNumDigitalChannels(void) {
  return kNumDigitalChannels;
}
int32_t HAL_GetNumPWMChannels(void) {
  return kNumPWMChannels;
}
int32_t HAL_GetNumDigitalPWMOutputs(void) {
  return kNumDigitalPWMOutputs;
}
int32_t HAL_GetNumEncoders(void) {
  return kNumEncoders;
}
int32_t HAL_GetNumInterrupts(void) {
  return kNumInterrupts;
}
int32_t HAL_GetNumRelayChannels(void) {
  return kNumRelayChannels;
}
int32_t HAL_GetNumRelayHeaders(void) {
  return kNumRelayHeaders;
}
int32_t HAL_GetNumPCMModules(void) {
  return kNumPCMModules;
}
int32_t HAL_GetNumSolenoidChannels(void) {
  return kNumSolenoidChannels;
}
int32_t HAL_GetNumPDPModules(void) {
  return kNumPDPModules;
}
int32_t HAL_GetNumPDPChannels(void) {
  return kNumPDPChannels;
}
int32_t HAL_GetNumDutyCycles(void) {
  return kNumDutyCycles;
}
int32_t HAL_GetNumAddressableLEDs(void) {
  return kNumAddressableLEDs;
}

}  // extern "C"
