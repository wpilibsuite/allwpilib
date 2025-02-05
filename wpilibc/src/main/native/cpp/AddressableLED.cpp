// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AddressableLED.h"

#include <hal/AddressableLED.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/PWM.h>
#include <hal/Ports.h>
#include <wpi/StackTrace.h>

#include "frc/Errors.h"

using namespace frc;

AddressableLED::AddressableLED(int port) : m_port{port} {
  int32_t status = 0;

  auto stack = wpi::GetStackTrace(1);
  m_pwmHandle =
      HAL_InitializePWMPort(HAL_GetPort(port), stack.c_str(), &status);
  FRC_CheckErrorStatus(status, "Port {}", port);
  if (m_pwmHandle == HAL_kInvalidHandle) {
    return;
  }

  m_handle = HAL_InitializeAddressableLED(m_pwmHandle, &status);
  FRC_CheckErrorStatus(status, "Port {}", port);
  if (m_handle == HAL_kInvalidHandle) {
    HAL_FreePWMPort(m_pwmHandle);
  }

  HAL_Report(HALUsageReporting::kResourceType_AddressableLEDs, port + 1);
}

void AddressableLED::SetColorOrder(AddressableLED::ColorOrder order) {
  int32_t status = 0;
  HAL_SetAddressableLEDColorOrder(
      m_handle, static_cast<HAL_AddressableLEDColorOrder>(order), &status);
  FRC_CheckErrorStatus(status, "Port {} Color order {}", m_port, order);
}

void AddressableLED::SetLength(int length) {
  int32_t status = 0;
  HAL_SetAddressableLEDLength(m_handle, length, &status);
  FRC_CheckErrorStatus(status, "Port {} length {}", m_port, length);
}

static_assert(sizeof(AddressableLED::LEDData) == sizeof(HAL_AddressableLEDData),
              "LED Structs MUST be the same size");

void AddressableLED::SetData(std::span<const LEDData> ledData) {
  int32_t status = 0;
  HAL_WriteAddressableLEDData(m_handle, ledData.data(), ledData.size(),
                              &status);
  FRC_CheckErrorStatus(status, "Port {}", m_port);
}

void AddressableLED::SetData(std::initializer_list<LEDData> ledData) {
  int32_t status = 0;
  HAL_WriteAddressableLEDData(m_handle, ledData.begin(), ledData.size(),
                              &status);
  FRC_CheckErrorStatus(status, "Port {}", m_port);
}

void AddressableLED::SetBitTiming(units::nanosecond_t highTime0,
                                  units::nanosecond_t lowTime0,
                                  units::nanosecond_t highTime1,
                                  units::nanosecond_t lowTime1) {
  int32_t status = 0;
  HAL_SetAddressableLEDBitTiming(
      m_handle, highTime0.to<int32_t>(), lowTime0.to<int32_t>(),
      highTime1.to<int32_t>(), lowTime1.to<int32_t>(), &status);
  FRC_CheckErrorStatus(status, "Port {}", m_port);
}

void AddressableLED::SetSyncTime(units::microsecond_t syncTime) {
  int32_t status = 0;
  HAL_SetAddressableLEDSyncTime(m_handle, syncTime.to<int32_t>(), &status);
  FRC_CheckErrorStatus(status, "Port {}", m_port);
}

void AddressableLED::Start() {
  int32_t status = 0;
  HAL_StartAddressableLEDOutput(m_handle, &status);
  FRC_CheckErrorStatus(status, "Port {}", m_port);
}

void AddressableLED::Stop() {
  int32_t status = 0;
  HAL_StopAddressableLEDOutput(m_handle, &status);
  FRC_CheckErrorStatus(status, "Port {}", m_port);
}

void AddressableLED::LEDData::SetHSV(int h, int s, int v) {
  SetLED(Color::FromHSV(h, s, v));
}
