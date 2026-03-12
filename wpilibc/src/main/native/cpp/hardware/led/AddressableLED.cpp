// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/led/AddressableLED.hpp"

#include <algorithm>

#include "wpi/hal/AddressableLED.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"

using namespace wpi;

AddressableLED::AddressableLED(int channel) : m_channel{channel} {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  auto stack = wpi::util::GetStackTrace(1);
  HAL_AddressableLEDHandle handle;
  HAL_Status status =
      HAL_InitializeAddressableLED(channel, stack.c_str(), &handle);
  m_handle = handle;
  WPILIB_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("IO", channel, "AddressableLED");
}

void AddressableLED::SetColorOrder(AddressableLED::ColorOrder order) {
  m_colorOrder = order;
}

void AddressableLED::SetStart(int start) {
  m_start = start;
  HAL_Status status = HAL_SetAddressableLEDStart(m_handle, start);
  WPILIB_CheckErrorStatus(status, "Channel {} start {}", m_channel, start);
}

void AddressableLED::SetLength(int length) {
  m_length = length;
  HAL_Status status = HAL_SetAddressableLEDLength(m_handle, length);
  WPILIB_CheckErrorStatus(status, "Channel {} length {}", m_channel, length);
}

static_assert(sizeof(AddressableLED::LEDData) == sizeof(HAL_AddressableLEDData),
              "LED Structs MUST be the same size");

void AddressableLED::SetData(std::span<const LEDData> ledData) {
  HAL_Status status = HAL_SetAddressableLEDData(
      m_start, std::min(static_cast<size_t>(m_length), ledData.size()),
      static_cast<HAL_AddressableLEDColorOrder>(m_colorOrder), ledData.data());
  WPILIB_CheckErrorStatus(status, "Port {}", m_channel);
}

void AddressableLED::SetData(std::initializer_list<LEDData> ledData) {
  SetData(std::span{ledData.begin(), ledData.end()});
}

void AddressableLED::SetGlobalData(int start, ColorOrder colorOrder,
                                   std::span<const LEDData> ledData) {
  HAL_Status status = HAL_SetAddressableLEDData(
      start, ledData.size(),
      static_cast<HAL_AddressableLEDColorOrder>(colorOrder), ledData.data());
  WPILIB_CheckErrorStatus(status, "");
}

void AddressableLED::LEDData::SetHSV(int h, int s, int v) {
  SetLED(wpi::util::Color::FromHSV(h, s, v));
}
