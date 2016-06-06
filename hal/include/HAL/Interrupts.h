/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

typedef HalHandle HalInterruptHandle;

extern "C" {
typedef void (*InterruptHandlerFunction)(uint32_t interruptAssertedMask,
                                         void* param);

HalInterruptHandle initializeInterrupts(bool watcher, int32_t* status);
void cleanInterrupts(HalInterruptHandle interrupt_handle, int32_t* status);

uint32_t waitForInterrupt(HalInterruptHandle interrupt_handle, double timeout,
                          bool ignorePrevious, int32_t* status);
void enableInterrupts(HalInterruptHandle interrupt_handle, int32_t* status);
void disableInterrupts(HalInterruptHandle interrupt_handle, int32_t* status);
double readRisingTimestamp(HalInterruptHandle interrupt_handle,
                           int32_t* status);
double readFallingTimestamp(HalInterruptHandle interrupt_handle,
                            int32_t* status);
void requestInterrupts(HalInterruptHandle interrupt_handle,
                       uint8_t routing_module, uint32_t routing_pin,
                       bool routing_analog_trigger, int32_t* status);
void attachInterruptHandler(HalInterruptHandle interrupt_handle,
                            InterruptHandlerFunction handler, void* param,
                            int32_t* status);
void setInterruptUpSourceEdge(HalInterruptHandle interrupt_handle,
                              bool risingEdge, bool fallingEdge,
                              int32_t* status);
}
