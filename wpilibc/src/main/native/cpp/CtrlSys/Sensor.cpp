/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/Sensor.h"

using namespace frc;

Sensor::Sensor(PIDSource& source) : m_source(source) {}

/**
 * Returns value from sensor
 */
double Sensor::GetOutput() { return m_source.PIDGet(); }
