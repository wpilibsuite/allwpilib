/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <IterativeRobot.h>
#include <PowerDistributionPanel.h>
#include <SmartDashboard/SmartDashboard.h>

/**
 * This is a sample program showing how to retrieve information from the Power
 * Distribution Panel via CAN. The information will be displayed under variables
 * through the SmartDashboard.
 */
class Robot : public frc::IterativeRobot {
public:
	void TeleopPeriodic() override {
		/* Get the current going through channel 7, in Amperes. The PDP
		 * returns the current in increments of 0.125A. At low currents
		 * the
		 * current readings tend to be less accurate.
		 */
		frc::SmartDashboard::PutNumber(
				"Current Channel 7", m_pdp.GetCurrent(7));

		/* Get the voltage going into the PDP, in Volts.
		 * The PDP returns the voltage in increments of 0.05 Volts.
		 */
		frc::SmartDashboard::PutNumber("Voltage", m_pdp.GetVoltage());

		// Retrieves the temperature of the PDP, in degrees Celsius.
		frc::SmartDashboard::PutNumber(
				"Temperature", m_pdp.GetTemperature());
	}

private:
	// Object for dealing with the Power Distribution Panel (PDP).
	frc::PowerDistributionPanel m_pdp;
};

START_ROBOT_CLASS(Robot)
