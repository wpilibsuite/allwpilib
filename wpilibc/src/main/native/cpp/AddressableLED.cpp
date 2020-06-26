/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AddressableLED.h"

#include <hal/AddressableLED.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/PWM.h>
#include <hal/Ports.h>

#include "frc/WPIErrors.h"

using namespace frc;

AddressableLED::AddressableLED(int port) {
  int32_t status = 0;

  m_pwmHandle = HAL_InitializePWMPort(HAL_GetPort(port), &status);
  wpi_setHALErrorWithRange(status, 0, HAL_GetNumPWMChannels(), port);
  if (m_pwmHandle == HAL_kInvalidHandle) {
    return;
  }

  m_handle = HAL_InitializeAddressableLED(m_pwmHandle, &status);
  wpi_setHALError(status);
  if (m_handle == HAL_kInvalidHandle) {
    HAL_FreePWMPort(m_pwmHandle, &status);
  }

  HAL_Report(HALUsageReporting::kResourceType_AddressableLEDs, port + 1);
}

AddressableLED::~AddressableLED() {
  HAL_FreeAddressableLED(m_handle);
  int32_t status = 0;
  HAL_FreePWMPort(m_pwmHandle, &status);
}

void AddressableLED::SetLength(int length) {
  int32_t status = 0;
  HAL_SetAddressableLEDLength(m_handle, length, &status);
  wpi_setHALError(status);
}

static_assert(sizeof(AddressableLED::LEDData) == sizeof(HAL_AddressableLEDData),
              "LED Structs MUST be the same size");

void AddressableLED::SetData(wpi::ArrayRef<LEDData> ledData) {
  int32_t status = 0;
  HAL_WriteAddressableLEDData(m_handle, ledData.begin(), ledData.size(),
                              &status);
  wpi_setHALError(status);
}

void AddressableLED::SetData(std::initializer_list<LEDData> ledData) {
  int32_t status = 0;
  HAL_WriteAddressableLEDData(m_handle, ledData.begin(), ledData.size(),
                              &status);
  wpi_setHALError(status);
}

void AddressableLED::SetBitTiming(units::nanosecond_t lowTime0,
                                  units::nanosecond_t highTime0,
                                  units::nanosecond_t lowTime1,
                                  units::nanosecond_t highTime1) {
  int32_t status = 0;
  HAL_SetAddressableLEDBitTiming(
      m_handle, lowTime0.to<int32_t>(), highTime0.to<int32_t>(),
      lowTime1.to<int32_t>(), highTime1.to<int32_t>(), &status);
  wpi_setHALError(status);
}

void AddressableLED::SetSyncTime(units::microsecond_t syncTime) {
  int32_t status = 0;
  HAL_SetAddressableLEDSyncTime(m_handle, syncTime.to<int32_t>(), &status);
  wpi_setHALError(status);
}

void AddressableLED::Start() {
  int32_t status = 0;
  HAL_StartAddressableLEDOutput(m_handle, &status);
  wpi_setHALError(status);
}

void AddressableLED::Stop() {
  int32_t status = 0;
  HAL_StopAddressableLEDOutput(m_handle, &status);
  wpi_setHALError(status);
}

void AddressableLED::LEDData::SetHSV(int h, int s, int v) {
  SetLED(Color::FromHSV(h, s, v));
}
