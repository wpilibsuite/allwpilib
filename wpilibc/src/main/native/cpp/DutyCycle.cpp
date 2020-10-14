/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DutyCycle.h"

#include <hal/DutyCycle.h>
#include <hal/FRCUsageReporting.h>

#include "frc/Base.h"
#include "frc/DigitalSource.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

DutyCycle::DutyCycle(DigitalSource* source)
    : m_source{source, NullDeleter<DigitalSource>()} {
  if (m_source == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitDutyCycle();
  }
}

DutyCycle::DutyCycle(DigitalSource& source)
    : m_source{&source, NullDeleter<DigitalSource>()} {
  InitDutyCycle();
}

DutyCycle::DutyCycle(std::shared_ptr<DigitalSource> source)
    : m_source{std::move(source)} {
  if (m_source == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitDutyCycle();
  }
}

DutyCycle::~DutyCycle() { HAL_FreeDutyCycle(m_handle); }

void DutyCycle::InitDutyCycle() {
  int32_t status = 0;
  m_handle =
      HAL_InitializeDutyCycle(m_source->GetPortHandleForRouting(),
                              static_cast<HAL_AnalogTriggerType>(
                                  m_source->GetAnalogTriggerTypeForRouting()),
                              &status);
  wpi_setHALError(status);
  int index = GetFPGAIndex();
  HAL_Report(HALUsageReporting::kResourceType_DutyCycle, index + 1);
  SendableRegistry::GetInstance().AddLW(this, "Duty Cycle", index);
}

int DutyCycle::GetFPGAIndex() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFPGAIndex(m_handle, &status);
  wpi_setHALError(status);
  return retVal;
}

int DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  wpi_setHALError(status);
  return retVal;
}

double DutyCycle::GetOutput() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  wpi_setHALError(status);
  return retVal;
}

unsigned int DutyCycle::GetOutputRaw() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutputRaw(m_handle, &status);
  wpi_setHALError(status);
  return retVal;
}

unsigned int DutyCycle::GetOutputScaleFactor() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutputScaleFactor(m_handle, &status);
  wpi_setHALError(status);
  return retVal;
}

int DutyCycle::GetSourceChannel() const { return m_source->GetChannel(); }

void DutyCycle::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Duty Cycle");
  builder.AddDoubleProperty(
      "Frequency", [this] { return this->GetFrequency(); }, nullptr);
  builder.AddDoubleProperty(
      "Output", [this] { return this->GetOutput(); }, nullptr);
}
