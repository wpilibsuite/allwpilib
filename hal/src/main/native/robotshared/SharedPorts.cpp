// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Ports.h"

#include "SharedPortsInternal.h"

using namespace hal;

extern "C" {

int32_t HAL_GetNumCTREPCMModules(void) {
  return kNumCTREPCMModules;
}
int32_t HAL_GetNumCTRESolenoidChannels(void) {
  return kNumCTRESolenoidChannels;
}
int32_t HAL_GetNumCTREPDPModules(void) {
  return kNumCTREPDPModules;
}
int32_t HAL_GetNumCTREPDPChannels(void) {
  return kNumCTREPDPChannels;
}
int32_t HAL_GetNumREVPDHModules(void) {
  return kNumREVPDHModules;
}
int32_t HAL_GetNumREVPDHChannels(void) {
  return kNumREVPDHChannels;
}
int32_t HAL_GetNumREVPHModules(void) {
  return kNumREVPHModules;
}
int32_t HAL_GetNumREVPHChannels(void) {
  return kNumREVPHChannels;
}

}  // extern "C"
