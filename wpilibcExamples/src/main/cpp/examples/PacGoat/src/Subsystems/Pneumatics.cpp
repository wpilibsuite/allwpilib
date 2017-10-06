#include "Pneumatics.h"

#include <LiveWindow/LiveWindow.h>

Pneumatics::Pneumatics() :
		frc::Subsystem("Pneumatics") {
	frc::LiveWindow::GetInstance()->AddSensor("Pneumatics", "Pressure Sensor",
			pressureSensor);
}

/**
 * No default command
 */
void Pneumatics::InitDefaultCommand() {

}

/**
 * Start the compressor going. The compressor automatically starts and stops as it goes above and below maximum pressure.
 */
void Pneumatics::Start() {
#ifndef SIMULATION
	compressor.Start();
#endif
}

/**
 * @return Whether or not the system is fully pressurized.
 */
bool Pneumatics::IsPressurized() {
#ifndef SIMULATION
	return kMaxPressure <= pressureSensor.GetVoltage();
#else
	return true;  // NOTE: Simulation always has full pressure
#endif
}

/**
 * Puts the pressure on the SmartDashboard.
 */
void Pneumatics::WritePressure() {
	frc::SmartDashboard::PutNumber("Pressure", pressureSensor.GetVoltage());
}
