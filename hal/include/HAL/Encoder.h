/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void* initializeEncoder(uint8_t port_a_module, uint32_t port_a_pin,
                        bool port_a_analog_trigger, uint8_t port_b_module,
                        uint32_t port_b_pin, bool port_b_analog_trigger,
                        bool reverseDirection, int32_t* index,
                        int32_t* status);  // TODO: fix routing
void freeEncoder(void* encoder_pointer, int32_t* status);
void resetEncoder(void* encoder_pointer, int32_t* status);
int32_t getEncoder(void* encoder_pointer, int32_t* status);  // Raw value
double getEncoderPeriod(void* encoder_pointer, int32_t* status);
void setEncoderMaxPeriod(void* encoder_pointer, double maxPeriod,
                         int32_t* status);
bool getEncoderStopped(void* encoder_pointer, int32_t* status);
bool getEncoderDirection(void* encoder_pointer, int32_t* status);
void setEncoderReverseDirection(void* encoder_pointer, bool reverseDirection,
                                int32_t* status);
void setEncoderSamplesToAverage(void* encoder_pointer,
                                uint32_t samplesToAverage, int32_t* status);
uint32_t getEncoderSamplesToAverage(void* encoder_pointer, int32_t* status);
void setEncoderIndexSource(void* encoder_pointer, uint32_t pin,
                           bool analogTrigger, bool activeHigh,
                           bool edgeSensitive, int32_t* status);
}
