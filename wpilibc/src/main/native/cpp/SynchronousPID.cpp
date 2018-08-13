/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SynchronousPID.h"

using namespace frc;

SynchronousPID::SynchronousPID(double Kp, double Ki, double Kd,
                               PIDSource& source, PIDOutput& output)
    : SynchronousPID(Kp, Ki, Kd, 0.0, source, output) {}

SynchronousPID::SynchronousPID(double Kp, double Ki, double Kd, double Kf,
                               PIDSource& source, PIDOutput& output)
    : PIDBase(Kp, Ki, Kd, Kf, source, output) {
  m_enabled = true;
}

void SynchronousPID::Calculate() { PIDBase::Calculate(); }
