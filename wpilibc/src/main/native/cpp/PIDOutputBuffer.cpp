/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PIDOutputBuffer.h"

using namespace frc;

void PIDOutputBuffer::PIDWrite(double output) { m_output = output; }

double PIDOutputBuffer::GetOutput() const { return m_output; }
