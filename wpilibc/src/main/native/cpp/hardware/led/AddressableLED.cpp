// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AddressableLED.h"
#include "wpi/hardware/led/AddressableLED.hpp"

#include <algorithm>

#include "wpi/hal/HALBase.h"
#include "wpi/hal/PWM.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"

using namespace frc;

AddressableLED::AddressableLED(int channel) : m_channel{channel} {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  int32_t status = 0;
  auto stack = wpi::GetStackTrace(1);
  m_handle = HAL_InitializeAddressableLED(channel, stack.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("IO", channel, "AddressableLED");
}

void AddressableLED::SetColorOrder(AddressableLED::ColorOrder order) {
  m_colorOrder = order;
}

void AddressableLED::SetStart(int start) {
  m_start = start;
  int32_t status = 0;
  HAL_SetAddressableLEDStart(m_handle, start, &status);
  FRC_CheckErrorStatus(status, "Channel {} start {}", m_channel, start);
}

void AddressableLED::SetLength(int length) {
  m_length = length;
  int32_t status = 0;
  HAL_SetAddressableLEDLength(m_handle, length, &status);
  FRC_CheckErrorStatus(status, "Channel {} length {}", m_channel, length);
}

static_assert(sizeof(AddressableLED::LEDData) == sizeof(HAL_AddressableLEDData),
              "LED Structs MUST be the same size");

void AddressableLED::SetData(std::span<const LEDData> ledData) {
  int32_t status = 0;
  HAL_SetAddressableLEDData(
      m_start, std::min(static_cast<size_t>(m_length), ledData.size()),
      static_cast<HAL_AddressableLEDColorOrder>(m_colorOrder), ledData.data(),
      &status);
  FRC_CheckErrorStatus(status, "Port {}", m_channel);
}

void AddressableLED::SetData(std::initializer_list<LEDData> ledData) {
  SetData(std::span{ledData.begin(), ledData.end()});
}

void AddressableLED::SetGlobalData(int start, ColorOrder colorOrder,
                                   std::span<const LEDData> ledData) {
  int32_t status = 0;
  HAL_SetAddressableLEDData(
      start, ledData.size(),
      static_cast<HAL_AddressableLEDColorOrder>(colorOrder), ledData.data(),
      &status);
  FRC_CheckErrorStatus(status, "");
}

void AddressableLED::LEDData::SetHSV(int h, int s, int v) {
  SetLED(Color::FromHSV(h, s, v));
}
