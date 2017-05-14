/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/RefInput.h"

#include "CtrlSys/Output.h"

using namespace frc;

RefInput::RefInput(double reference) { Set(reference); }

void RefInput::SetCallback(Output& output) {
  m_callback = std::bind(&Output::OutputFunc, std::ref(output));
}

/**
 * Returns value of reference input.
 */
double RefInput::GetOutput() {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_reference;
}

/**
 * Returns value of reference input.
 */
double RefInput::GetOutput() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_reference;
}

/**
 * Sets reference input.
 */
void RefInput::Set(double reference) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_reference = reference;
  }

  m_callback();
}
