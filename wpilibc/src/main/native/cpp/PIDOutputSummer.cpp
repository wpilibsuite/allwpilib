/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PIDOutputSummer.h"

using namespace frc;

/**
 * Appends buffer and associated sign to array.
 *
 * @param output The output for the sum.
 */
PIDOutputSummer::PIDOutputSummer(PIDOutput& output) : m_output(output) {}

/**
 * Starts periodic output.
 *
 * @param period The loop time for doing calculations in seconds. The default
 *               period is 50ms.
 */
void PIDOutputSummer::Enable(double period) {
  m_notifier.StartPeriodic(period);
}

/**
 * Stops periodic output.
 */
void PIDOutputSummer::Disable() { m_notifier.Stop(); }

void PIDOutputSummer::SetOutput() {
  double sum = 0.0;

  for (auto& buffer : m_buffers) {
    if (buffer.second) {
      sum += buffer.first.GetOutput();
    } else {
      sum -= buffer.first.GetOutput();
    }
  }

  m_output.PIDWrite(sum);
}
