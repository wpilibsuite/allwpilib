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

void setPWMConfig(HalDigitalHandle pwm_port_handle, double maxPwm,
                  double deadbandMaxPwm, double centerPwm,
                  double deadbandMinPwm, double minPwm, int32_t* status);
void setPWMConfigRaw(HalDigitalHandle pwm_port_handle, int32_t maxPwm,
                     int32_t deadbandMaxPwm, int32_t centerPwm,
                     int32_t deadbandMinPwm, int32_t minPwm, int32_t* status);
void getPWMConfigRaw(HalDigitalHandle pwm_port_handle, int32_t* maxPwm,
                     int32_t* deadbandMaxPwm, int32_t* centerPwm,
                     int32_t* deadbandMinPwm, int32_t* minPwm, int32_t* status);
void setPWMEliminateDeadband(HalDigitalHandle pwm_port_handle,
                             uint8_t eliminateDeadband, int32_t* status);
uint8_t getPWMEliminateDeadband(HalDigitalHandle pwm_port_handle,
                                int32_t* status);
void setPWMRaw(HalDigitalHandle pwm_port_handle, uint16_t value,
               int32_t* status);
void setPWMSpeed(HalDigitalHandle pwm_port_handle, float speed,
                 int32_t* status);
void setPWMPosition(HalDigitalHandle pwm_port_handle, float position,
                    int32_t* status);
void setPWMDisabled(HalDigitalHandle pwm_port_handle, int32_t* status);
uint16_t getPWMRaw(HalDigitalHandle pwm_port_handle, int32_t* status);
float getPWMSpeed(HalDigitalHandle pwm_port_handle, int32_t* status);
float getPWMPosition(HalDigitalHandle pwm_port_handle, int32_t* status);
void latchPWMZero(HalDigitalHandle pwm_port_handle, int32_t* status);
void setPWMPeriodScale(HalDigitalHandle pwm_port_handle, uint32_t squelchMask,
                       int32_t* status);
uint16_t getLoopTiming(int32_t* status);
}
