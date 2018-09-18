/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DigitalGlitchFilter.h"

#include <algorithm>
#include <array>
#include <utility>

#include <hal/Constants.h>
#include <hal/DIO.h>
#include <hal/HAL.h>

#include "frc/Counter.h"
#include "frc/Encoder.h"
#include "frc/SensorUtil.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"

using namespace frc;

std::array<bool, 3> DigitalGlitchFilter::m_filterAllocated = {
    {false, false, false}};
wpi::mutex DigitalGlitchFilter::m_mutex;

DigitalGlitchFilter::DigitalGlitchFilter() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  auto index =
      std::find(m_filterAllocated.begin(), m_filterAllocated.end(), false);
  wpi_assert(index != m_filterAllocated.end());

  m_channelIndex = std::distance(m_filterAllocated.begin(), index);
  *index = true;

  HAL_Report(HALUsageReporting::kResourceType_DigitalGlitchFilter,
             m_channelIndex);
  SetName("DigitalGlitchFilter", m_channelIndex);
}

DigitalGlitchFilter::~DigitalGlitchFilter() {
  if (m_channelIndex >= 0) {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_filterAllocated[m_channelIndex] = false;
  }
}

DigitalGlitchFilter::DigitalGlitchFilter(DigitalGlitchFilter&& rhs)
    : ErrorBase(std::move(rhs)), SendableBase(std::move(rhs)) {
  std::swap(m_channelIndex, rhs.m_channelIndex);
}

DigitalGlitchFilter& DigitalGlitchFilter::operator=(DigitalGlitchFilter&& rhs) {
  ErrorBase::operator=(std::move(rhs));
  SendableBase::operator=(std::move(rhs));

  std::swap(m_channelIndex, rhs.m_channelIndex);

  return *this;
}

void DigitalGlitchFilter::Add(DigitalSource* input) {
  DoAdd(input, m_channelIndex + 1);
}

void DigitalGlitchFilter::DoAdd(DigitalSource* input, int requestedIndex) {
  // Some sources from Counters and Encoders are null. By pushing the check
  // here, we catch the issue more generally.
  if (input) {
    // We don't support GlitchFilters on AnalogTriggers.
    if (input->IsAnalogTrigger()) {
      wpi_setErrorWithContext(
          -1, "Analog Triggers not supported for DigitalGlitchFilters");
      return;
    }
    int32_t status = 0;
    HAL_SetFilterSelect(input->GetPortHandleForRouting(), requestedIndex,
                        &status);
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

    // Validate that we set it correctly.
    int actualIndex =
        HAL_GetFilterSelect(input->GetPortHandleForRouting(), &status);
    wpi_assertEqual(actualIndex, requestedIndex);

    HAL_Report(HALUsageReporting::kResourceType_DigitalInput,
               input->GetChannel());
  }
}

void DigitalGlitchFilter::Add(Encoder* input) {
  Add(input->m_aSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Add(input->m_bSource.get());
}

void DigitalGlitchFilter::Add(Counter* input) {
  Add(input->m_upSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Add(input->m_downSource.get());
}

void DigitalGlitchFilter::Remove(DigitalSource* input) { DoAdd(input, 0); }

void DigitalGlitchFilter::Remove(Encoder* input) {
  Remove(input->m_aSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Remove(input->m_bSource.get());
}

void DigitalGlitchFilter::Remove(Counter* input) {
  Remove(input->m_upSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Remove(input->m_downSource.get());
}

void DigitalGlitchFilter::SetPeriodCycles(int fpgaCycles) {
  int32_t status = 0;
  HAL_SetFilterPeriod(m_channelIndex, fpgaCycles, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DigitalGlitchFilter::SetPeriodNanoSeconds(uint64_t nanoseconds) {
  int32_t status = 0;
  int fpgaCycles =
      nanoseconds * HAL_GetSystemClockTicksPerMicrosecond() / 4 / 1000;
  HAL_SetFilterPeriod(m_channelIndex, fpgaCycles, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int DigitalGlitchFilter::GetPeriodCycles() {
  int32_t status = 0;
  int fpgaCycles = HAL_GetFilterPeriod(m_channelIndex, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  return fpgaCycles;
}

uint64_t DigitalGlitchFilter::GetPeriodNanoSeconds() {
  int32_t status = 0;
  int fpgaCycles = HAL_GetFilterPeriod(m_channelIndex, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  return static_cast<uint64_t>(fpgaCycles) * 1000L /
         static_cast<uint64_t>(HAL_GetSystemClockTicksPerMicrosecond() / 4);
}

void DigitalGlitchFilter::InitSendable(SendableBuilder&) {}
