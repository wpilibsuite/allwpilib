/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/controller/Controller.h"

using namespace frc::experimental;

Controller::Controller(double period) : m_period(period) {}

double Controller::GetPeriod() const { return m_period; }
