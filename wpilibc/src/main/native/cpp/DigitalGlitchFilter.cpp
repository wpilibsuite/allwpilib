// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalGlitchFilter.h"

#include <algorithm>
#include <array>
#include <utility>

#include <hal/Constants.h>
#include <hal/DIO.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Counter.h"
#include "frc/Encoder.h"
#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

std::array<bool, 3> DigitalGlitchFilter::m_filterAllocated = {
    {false, false, false}};
wpi::mutex DigitalGlitchFilter::m_mutex;

DigitalGlitchFilter::DigitalGlitchFilter() {
  m_channelIndex = AllocateFilterIndex();
  if (m_channelIndex < 0) {
    throw FRC_MakeError(err::NoAvailableResources,
                        "No filters available to allocate.");
  }
  HAL_Report(HALUsageReporting::kResourceType_DigitalGlitchFilter,
             m_channelIndex + 1);
  wpi::SendableRegistry::AddLW(this, "DigitalGlitchFilter", m_channelIndex);
}

DigitalGlitchFilter::~DigitalGlitchFilter() {
  if (m_channelIndex >= 0) {
    std::scoped_lock lock(m_mutex);
    m_filterAllocated[m_channelIndex] = false;
  }
}

int DigitalGlitchFilter::AllocateFilterIndex() {
  std::scoped_lock lock{m_mutex};
  auto filter =
      std::find(m_filterAllocated.begin(), m_filterAllocated.end(), false);

  if (filter == m_filterAllocated.end()) {
    return -1;
  }
  *filter = true;
  return std::distance(m_filterAllocated.begin(), filter);
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
      throw FRC_MakeError(
          -1, "Analog Triggers not supported for DigitalGlitchFilters");
    }
    int32_t status = 0;
    HAL_SetFilterSelect(input->GetPortHandleForRouting(), requestedIndex,
                        &status);
    FRC_CheckErrorStatus(status, "requested index {}", requestedIndex);

    // Validate that we set it correctly.
    int actualIndex =
        HAL_GetFilterSelect(input->GetPortHandleForRouting(), &status);
    FRC_CheckErrorStatus(status, "requested index {}", requestedIndex);
    FRC_AssertMessage(actualIndex == requestedIndex,
                      "HAL_SetFilterSelect({}) failed -> {}", requestedIndex,
                      actualIndex);
  }
}

void DigitalGlitchFilter::Add(Encoder* input) {
  Add(input->m_aSource.get());
  Add(input->m_bSource.get());
}

void DigitalGlitchFilter::Add(Counter* input) {
  Add(input->m_upSource.get());
  Add(input->m_downSource.get());
}

void DigitalGlitchFilter::Remove(DigitalSource* input) {
  DoAdd(input, 0);
}

void DigitalGlitchFilter::Remove(Encoder* input) {
  Remove(input->m_aSource.get());
  Remove(input->m_bSource.get());
}

void DigitalGlitchFilter::Remove(Counter* input) {
  Remove(input->m_upSource.get());
  Remove(input->m_downSource.get());
}

void DigitalGlitchFilter::SetPeriodCycles(int fpgaCycles) {
  int32_t status = 0;
  HAL_SetFilterPeriod(m_channelIndex, fpgaCycles, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channelIndex);
}

void DigitalGlitchFilter::SetPeriodNanoSeconds(uint64_t nanoseconds) {
  int32_t status = 0;
  int fpgaCycles =
      nanoseconds * HAL_GetSystemClockTicksPerMicrosecond() / 4 / 1000;
  HAL_SetFilterPeriod(m_channelIndex, fpgaCycles, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channelIndex);
}

int DigitalGlitchFilter::GetPeriodCycles() {
  int32_t status = 0;
  int fpgaCycles = HAL_GetFilterPeriod(m_channelIndex, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channelIndex);
  return fpgaCycles;
}

uint64_t DigitalGlitchFilter::GetPeriodNanoSeconds() {
  int32_t status = 0;
  int fpgaCycles = HAL_GetFilterPeriod(m_channelIndex, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channelIndex);
  return static_cast<uint64_t>(fpgaCycles) * 1000L /
         static_cast<uint64_t>(HAL_GetSystemClockTicksPerMicrosecond() / 4);
}

void DigitalGlitchFilter::InitSendable(wpi::SendableBuilder&) {}
