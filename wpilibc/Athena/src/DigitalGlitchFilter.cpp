/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <array>

#include "DigitalGlitchFilter.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "Encoder.h"
#include "Counter.h"
#include "Utility.h"

std::array<bool, 3> DigitalGlitchFilter::m_filterAllocated = {{false, false,
                                                               false}};
priority_mutex DigitalGlitchFilter::m_mutex;

DigitalGlitchFilter::DigitalGlitchFilter() {
  std::lock_guard<priority_mutex> sync(m_mutex);
  auto index =
      std::find(m_filterAllocated.begin(), m_filterAllocated.end(), false);
  wpi_assert(index != m_filterAllocated.end());

  m_channelIndex = std::distance(m_filterAllocated.begin(), index);
  *index = true;

  HALReport(HALUsageReporting::kResourceType_DigitalFilter, m_channelIndex);
}

DigitalGlitchFilter::~DigitalGlitchFilter() {
  if (m_channelIndex >= 0) {
    std::lock_guard<priority_mutex> sync(m_mutex);
    m_filterAllocated[m_channelIndex] = false;
  }
}

/**
 * Assigns the DigitalSource to this glitch filter.
 *
 * @param input The DigitalSource to add.
 */
void DigitalGlitchFilter::Add(DigitalSource *input) {
  DoAdd(input, m_channelIndex + 1);
}

void DigitalGlitchFilter::DoAdd(DigitalSource *input, int requested_index) {
  // Some sources from Counters and Encoders are null.  By pushing the check
  // here, we catch the issue more generally.
  if (input) {
    int32_t status = 0;
    setFilterSelect(m_digital_ports[input->GetChannelForRouting()],
                    requested_index, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));

    // Validate that we set it correctly.
    int actual_index = getFilterSelect(
        m_digital_ports[input->GetChannelForRouting()], &status);
    wpi_assertEqual(actual_index, requested_index);

    HALReport(HALUsageReporting::kResourceType_DigitalInput,
              input->GetChannelForRouting());
  }
}

/**
 * Assigns the Encoder to this glitch filter.
 *
 * @param input The Encoder to add.
 */
void DigitalGlitchFilter::Add(Encoder *input) {
  Add(input->m_aSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Add(input->m_bSource.get());
}

/**
 * Assigns the Counter to this glitch filter.
 *
 * @param input The Counter to add.
 */
void DigitalGlitchFilter::Add(Counter *input) {
  Add(input->m_upSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Add(input->m_downSource.get());
}

/**
 * Removes a digital input from this filter.
 *
 * Removes the DigitalSource from this glitch filter and re-assigns it to
 * the default filter.
 *
 * @param input The DigitalSource to remove.
 */
void DigitalGlitchFilter::Remove(DigitalSource *input) {
  DoAdd(input, 0);
}

/**
 * Removes an encoder from this filter.
 *
 * Removes the Encoder from this glitch filter and re-assigns it to
 * the default filter.
 *
 * @param input The Encoder to remove.
 */
void DigitalGlitchFilter::Remove(Encoder *input) {
  Remove(input->m_aSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Remove(input->m_bSource.get());
}

/**
 * Removes a counter from this filter.
 *
 * Removes the Counter from this glitch filter and re-assigns it to
 * the default filter.
 *
 * @param input The Counter to remove.
 */
void DigitalGlitchFilter::Remove(Counter *input) {
  Remove(input->m_upSource.get());
  if (StatusIsFatal()) {
    return;
  }
  Remove(input->m_downSource.get());
}

/**
 * Sets the number of cycles that the input must not change state for.
 *
 * @param fpga_cycles The number of FPGA cycles.
 */
void DigitalGlitchFilter::SetPeriodCycles(uint32_t fpga_cycles) {
  int32_t status = 0;
  setFilterPeriod(m_channelIndex, fpga_cycles, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Sets the number of nanoseconds that the input must not change state for.
 *
 * @param nanoseconds The number of nanoseconds.
 */
void DigitalGlitchFilter::SetPeriodNanoSeconds(uint64_t nanoseconds) {
  int32_t status = 0;
  uint32_t fpga_cycles =
      nanoseconds * kSystemClockTicksPerMicrosecond / 4 / 1000;
  setFilterPeriod(m_channelIndex, fpga_cycles, &status);

  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Gets the number of cycles that the input must not change state for.
 *
 * @return The number of cycles.
 */
uint32_t DigitalGlitchFilter::GetPeriodCycles() {
  int32_t status = 0;
  uint32_t fpga_cycles = getFilterPeriod(m_channelIndex, &status);

  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  return fpga_cycles;
}

/**
 * Gets the number of nanoseconds that the input must not change state for.
 *
 * @return The number of nanoseconds.
 */
uint64_t DigitalGlitchFilter::GetPeriodNanoSeconds() {
  int32_t status = 0;
  uint32_t fpga_cycles = getFilterPeriod(m_channelIndex, &status);

  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  return static_cast<uint64_t>(fpga_cycles) * 1000L /
         static_cast<uint64_t>(kSystemClockTicksPerMicrosecond / 4);
}
