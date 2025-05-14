// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {

constexpr int32_t NUM_CAN_BUSES = 5;
constexpr int32_t NUM_SMART_IO = 6;
constexpr int32_t NUM_I2C_BUSES = 2;
constexpr int32_t NUM_ACCUMULATORS = 0;
constexpr int32_t NUM_ANALOG_INPUTS = 8;
constexpr int32_t NUM_ANALOG_OUTPUTS = 0;
constexpr int32_t NUM_COUNTERS = 0;
constexpr int32_t NUM_DIGITAL_HEADERS = 10;
constexpr int32_t NUM_DIGITAL_MXP_CHANNELS = 16;
constexpr int32_t NUM_DIGITAL_SPI_PORT_CHANNELS = 5;
constexpr int32_t NUM_PWM_HEADERS = 0;
constexpr int32_t NUM_DIGITAL_CHANNELS = NUM_DIGITAL_HEADERS +
                                         NUM_DIGITAL_MXP_CHANNELS +
                                         NUM_DIGITAL_SPI_PORT_CHANNELS;
constexpr int32_t NUM_PWM_CHANNELS = 0 + NUM_PWM_HEADERS;
constexpr int32_t NUM_DIGITAL_PWM_OUTPUTS = 0;
constexpr int32_t NUM_ENCODERS = 0;
constexpr int32_t NUM_INTERRUPTS = 0;
constexpr int32_t NUM_RELAY_CHANNELS = 8;
constexpr int32_t NUM_RELAY_HEADERS = NUM_RELAY_CHANNELS / 2;
constexpr int32_t NUM_CTRE_PCM_MODULES = 63;
constexpr int32_t NUM_CTRE_SOLENOID_CHANNELS = 8;
constexpr int32_t NUM_CTRE_PDP_MODULES = 63;
constexpr int32_t NUM_CTRE_PDP_CHANNELS = 16;
constexpr int32_t NUM_REV_PDH_MODULES = 63;
constexpr int32_t NUM_REV_PDH_CHANNELS = 24;
constexpr int32_t NUM_DUTY_CYCLES = 0;
constexpr int32_t NUM_ADDRESSABLE_LEDS = 0;
constexpr int32_t NUM_REV_PH_MODULES = 63;
constexpr int32_t NUM_REV_PH_CHANNELS = 16;

}  // namespace hal
