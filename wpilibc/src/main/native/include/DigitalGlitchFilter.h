/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <array>

#include <HAL/cpp/priority_mutex.h>

#include "DigitalSource.h"

namespace frc {

class Encoder;
class Counter;

/**
 * Class to enable glitch filtering on a set of digital inputs.
 * This class will manage adding and removing digital inputs from a FPGA glitch
 * filter. The filter lets the user configure the time that an input must remain
 * high or low before it is classified as high or low.
 */
class DigitalGlitchFilter : public SensorBase {
 public:
  DigitalGlitchFilter();
  ~DigitalGlitchFilter();

  void Add(DigitalSource* input);
  void Add(Encoder* input);
  void Add(Counter* input);

  void Remove(DigitalSource* input);
  void Remove(Encoder* input);
  void Remove(Counter* input);

  void SetPeriodCycles(int fpga_cycles);
  void SetPeriodNanoSeconds(uint64_t nanoseconds);

  int GetPeriodCycles();
  uint64_t GetPeriodNanoSeconds();

 private:
  // Sets the filter for the input to be the requested index.  A value of 0
  // disables the filter, and the filter value must be between 1 and 3,
  // inclusive.
  void DoAdd(DigitalSource* input, int requested_index);

  int m_channelIndex = -1;
  static hal::priority_mutex m_mutex;
  static std::array<bool, 3> m_filterAllocated;
};

}  // namespace frc
