// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace wpi::hal {

constexpr int32_t NUM_CAN_BUSES = 25;
constexpr int32_t NUM_SMART_IO = 6;
constexpr int32_t NUM_I2C_BUSES = 2;
constexpr int32_t NUM_ACCUMULATORS = 0;
constexpr int32_t NUM_ANALOG_INPUTS = NUM_SMART_IO;
constexpr int32_t NUM_ANALOG_OUTPUTS = 0;
constexpr int32_t NUM_COUNTERS = 0;
constexpr int32_t NUM_DIGITAL_SPI_PORT_CHANNELS = 0;
constexpr int32_t NUM_DIGITAL_CHANNELS = NUM_SMART_IO;
constexpr int32_t NUM_PWM_CHANNELS = NUM_SMART_IO;
constexpr int32_t NUM_DIGITAL_PWM_OUTPUTS = 0;
constexpr int32_t NUM_ENCODERS = NUM_SMART_IO / 2;
constexpr int32_t NUM_INTERRUPTS = 0;
constexpr int32_t NUM_RELAY_CHANNELS = 0;
constexpr int32_t NUM_CTREPCM_MODULES = 63;
constexpr int32_t NUM_CTRE_SOLENOID_CHANNELS = 8;
constexpr int32_t NUM_CTREPDP_MODULES = 63;
constexpr int32_t NUM_CTREPDP_CHANNELS = 16;
constexpr int32_t NUM_REVPDH_MODULES = 63;
constexpr int32_t NUM_REVPDH_CHANNELS = 24;
constexpr int32_t NUM_DUTY_CYCLES = 0;
constexpr int32_t NUM_ADDRESSABLE_LE_DS = 6;
constexpr int32_t NUM_REVPH_MODULES = 63;
constexpr int32_t NUM_REVPH_CHANNELS = 16;

}  // namespace wpi::hal
