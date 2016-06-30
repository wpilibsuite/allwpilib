/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Handles.h"

extern "C" {
HalDigitalHandle initializePWMPort(HalPortHandle port_handle, int32_t* status);
void freePWMPort(HalDigitalHandle pwm_port_handle, int32_t* status);

bool checkPWMChannel(uint8_t pin);

void setPWM(HalDigitalHandle pwm_port_handle, unsigned short value,
            int32_t* status);
unsigned short getPWM(HalDigitalHandle pwm_port_handle, int32_t* status);
void latchPWMZero(HalDigitalHandle pwm_port_handle, int32_t* status);
void setPWMPeriodScale(HalDigitalHandle pwm_port_handle, uint32_t squelchMask,
                       int32_t* status);
uint16_t getLoopTiming(int32_t* status);
}
