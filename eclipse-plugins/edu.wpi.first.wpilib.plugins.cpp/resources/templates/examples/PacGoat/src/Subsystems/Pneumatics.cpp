#include "Pneumatics.h"

Pneumatics::Pneumatics() :
		Subsystem("Pneumatics"),
		pressureSensor(new AnalogInput(3))
{
	#ifdef REAL
		compressor = new Compressor(uint8_t(1)); // TODO: (1, 14, 1, 8);
	#endif

	LiveWindow::GetInstance().AddSensor("Pneumatics", "Pressure Sensor", pressureSensor);
}

/**
 * No default command
 */
void Pneumatics::InitDefaultCommand() {}

/**
 * Start the compressor going. The compressor automatically starts and stops as it goes above and below maximum pressure.
 */
void Pneumatics::Start() {
    #ifdef REAL
		compressor->Start();
    #endif
}

/**
 * @return Whether or not the system is fully pressurized.
 */
bool Pneumatics::IsPressurized() {
    #ifdef REAL
		return MAX_PRESSURE <= pressureSensor->GetVoltage();
    #else
		return true; // NOTE: Simulation always has full pressure
	#endif
}

/**
 * Puts the pressure on the SmartDashboard.
 */
void Pneumatics::WritePressure() {
	SmartDashboard::PutNumber("Pressure", pressureSensor->GetVoltage());
}
