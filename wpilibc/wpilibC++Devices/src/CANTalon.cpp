/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "CANTalon.h"
#include "ctre/CanTalonSRX.h"

/**
 * Constructor for the CANTalon device.
 * @param deviceNumber The CAN ID of the Talon SRX
 */
CANTalon::CANTalon(uint8_t deviceNumber) {
	m_impl = new CanTalonSRX(deviceNumber);
}

CANTalon::~CANTalon() {
	delete m_impl;
}

void CANTalon::Set(float value, uint8_t syncGroup) {
}

float CANTalon::Get() {
	return 0.0f;
}

void CANTalon::Disable() {
}
