/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/DigitalIOButton.h"
#include "DriverStation.h"

const bool DigitalIOButton::kActiveState = false;

DigitalIOButton::DigitalIOButton(int port) :
	m_port(port)
{
}

bool DigitalIOButton::Get()
{
	return DriverStation::GetInstance()->GetEnhancedIO().GetDigital(m_port) == kActiveState;
}
