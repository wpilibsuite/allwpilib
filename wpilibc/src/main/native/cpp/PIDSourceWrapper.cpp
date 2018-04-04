/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PIDSourceWrapper.h"

using namespace frc;

PIDSourceWrapper::PIDSourceWrapper(std::function<double()> func) {
  m_func = func;
}

double PIDSourceWrapper::PIDGet() { return m_func(); }
