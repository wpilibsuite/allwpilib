/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/Output.h"

#include "DriverStation.h"

using namespace frc;

/**
 * Calls PIDWrite() on the output at a regular interval.
 *
 * @param input the node that is used to get values
 * @param output the PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations.
 */
Output::Output(INode& input, PIDOutput& output, double period)
    : m_input(input),
      m_output(output),
      m_period(period),
      m_thread(&Output::OutputFunc, this) {
  m_input.SetCallback(*this);
}

/**
 * Starts closed loop control.
 */
void Output::Enable() { m_thread.StartPeriodic(m_period); }

/**
 * Stops closed loop control.
 */
void Output::Disable() {
  m_thread.Stop();

  m_output.PIDWrite(0.0);
}

/**
 * Set minimum and maximum control action. U designates control action.
 *
 * @param minU minimum control action
 * @param maxU maximum control action
 */
void Output::SetRange(double minU, double maxU) {
  std::lock_guard<std::mutex> lock(m_mutex);

  m_minU = minU;
  m_maxU = maxU;
}

void Output::OutputFunc() {
  double controlAction = m_input.GetOutput();

  std::lock_guard<std::mutex> lock(m_mutex);

  if (controlAction > m_maxU) {
    m_output.PIDWrite(m_maxU);
  } else if (controlAction < m_minU) {
    m_output.PIDWrite(m_minU);
  } else {
    m_output.PIDWrite(controlAction);
  }
}
