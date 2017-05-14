/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/OutputGroup.h"

using namespace frc;

/**
 * Appends output to array.
 *
 * @param output the PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations.
 */
OutputGroup::OutputGroup(Output& output)
    : m_thread(&OutputGroup::OutputFunc, this) {
  m_outputs.emplace_back(output);
}

/**
 * Starts closed loop control.
 *
 * @param period the loop time for doing calculations.
 */
void OutputGroup::Enable(double period) { m_thread.StartPeriodic(period); }

/**
 * Stops closed loop control.
 */
void OutputGroup::Disable() {
  m_thread.Stop();

  for (auto& output : m_outputs) {
    output.get().Disable();
  }
}

void OutputGroup::OutputFunc() {
  for (auto& output : m_outputs) {
    output.get().OutputFunc();
  }
}
