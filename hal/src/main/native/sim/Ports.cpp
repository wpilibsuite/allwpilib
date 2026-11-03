// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Ports.h"

#include "PortsInternal.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializePorts() {}
}  // namespace wpi::hal::init

extern "C" {
int32_t HAL_GetNumCanBuses(void) {
  return NUM_CAN_BUSES;
}
int32_t HAL_GetNumSmartIo(void) {
  return NUM_SMART_IO;
}
int32_t HAL_GetNumAnalogInputs(void) {
  return NUM_ANALOG_INPUTS;
}
int32_t HAL_GetNumCounters(void) {
  return NUM_COUNTERS;
}
int32_t HAL_GetNumDigitalHeaders(void) {
  return NUM_DIGITAL_HEADERS;
}
int32_t HAL_GetNumPWMHeaders(void) {
  return NUM_PWM_HEADERS;
}
int32_t HAL_GetNumDigitalChannels(void) {
  return NUM_DIGITAL_CHANNELS;
}
int32_t HAL_GetNumPWMChannels(void) {
  return NUM_PWM_CHANNELS;
}
int32_t HAL_GetNumDigitalPWMOutputs(void) {
  return NUM_DIGITAL_PWM_OUTPUTS;
}
int32_t HAL_GetNumEncoders(void) {
  return NUM_ENCODERS;
}
int32_t HAL_GetNumInterrupts(void) {
  return NUM_INTERRUPTS;
}
int32_t HAL_GetNumCTREPCMModules(void) {
  return NUM_CTREPCM_MODULES;
}
int32_t HAL_GetNumCTRESolenoidChannels(void) {
  return NUM_CTRE_SOLENOID_CHANNELS;
}
int32_t HAL_GetNumCTREPDPModules(void) {
  return NUM_CTREPDP_MODULES;
}
int32_t HAL_GetNumCTREPDPChannels(void) {
  return NUM_CTREPDP_CHANNELS;
}
int32_t HAL_GetNumREVPDHModules(void) {
  return NUM_REVPDH_MODULES;
}
int32_t HAL_GetNumREVPDHChannels(void) {
  return NUM_REVPDH_CHANNELS;
}
int32_t HAL_GetNumREVPHModules(void) {
  return NUM_REVPH_MODULES;
}
int32_t HAL_GetNumREVPHChannels(void) {
  return NUM_REVPH_CHANNELS;
}
int32_t HAL_GetNumDutyCycles(void) {
  return NUM_DUTY_CYCLES;
}
int32_t HAL_GetNumAddressableLEDs(void) {
  return NUM_ADDRESSABLE_LE_DS;
}
}  // extern "C"
