/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/AnalogIOButton.h"
#include "DriverStation.h"

const double AnalogIOButton::kThreshold = 0.5;

AnalogIOButton::AnalogIOButton(int port) :
	m_port(port)
{
}

bool AnalogIOButton::Get()
{
	return DriverStation::GetInstance()->GetEnhancedIO().GetAnalogIn(m_port) < kThreshold;
}
