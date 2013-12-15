/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2013. All Rights Reserved.  			      */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

#include "Analog.h"
#include "Digital.h"
#include "Solenoid.h"
#include "Watchdog.h"
#include "Notifier.h"
#include "Interrupts.h"
#include "Errors.h"

#include "Utilities.h"
#include "Semaphore.h"
#include "Task.h"

#ifndef HAL_H
#define HAL_H


inline float intToFloat(int value) {
  return *(float*) &value;
}

inline int floatToInt(float value) {
  return *(int*) &value;
}

extern "C" {
  extern const uint32_t dio_kNumSystems;
  extern const uint32_t solenoid_kNumDO7_0Elements;
  extern const uint32_t interrupt_kNumSystems;
  extern const uint32_t kSystemClockTicksPerMicrosecond;
  
  void* getPort(uint8_t pin);
  void* getPortWithModule(uint8_t module, uint8_t pin);
  const char* getHALErrorMessage(int32_t code);

  uint16_t getFPGAVersion(int32_t *status);
  uint32_t getFPGARevision(int32_t *status);
  uint32_t getFPGATime(int32_t *status);

  void setFPGALED(uint32_t state, int32_t *status);
  int32_t getFPGALED(int32_t *status);
}

// TODO: HACKS for now...
extern "C" {
  void FRC_NetworkCommunication_Reserve();

  void NumericArrayResize();
  void RTSetCleanupProc();
  void EDVR_CreateReference();
  void Occur();
}
#endif
