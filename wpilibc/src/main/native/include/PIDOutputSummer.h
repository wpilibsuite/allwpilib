/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <utility>
#include <vector>

#include "Notifier.h"
#include "PIDOutput.h"
#include "PIDOutputBuffer.h"

namespace frc {

/**
 * Takes an arbitrary list of PIDOutputBuffers and outputs the sum of their
 * outputs to a PIDOutput.
 */
class PIDOutputSummer {
 public:
  template <class... PIDOutputBuffers>
  PIDOutputSummer(PIDOutput& output, PIDOutputBuffer& buffer, bool positive,
                  PIDOutputBuffers&&... buffers);
  explicit PIDOutputSummer(PIDOutput& output);

  void Enable(double period = 0.05);
  void Disable();

 private:
  /**
   * First argument is buffer.
   * Second argument is whether to add or subtract its output.
   */
  std::vector<std::pair<PIDOutputBuffer&, bool>> m_buffers;

  PIDOutput& m_output;
  Notifier m_notifier{&PIDOutputSummer::SetOutput, this};

  void SetOutput();
};

}  // namespace frc

#include "PIDOutputSummer.inc"
