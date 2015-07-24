#ifndef Pneumatics_H
#define Pneumatics_H

#include "Commands/Subsystem.h"
#include "WPILib.h"

/**
 * The Pneumatics subsystem contains the compressor and a pressure sensor.
 *
 * NOTE: The simulator currently doesn't support the compressor or pressure sensors.
 */
class Pneumatics: public Subsystem
{
private:
  std::shared_ptr<AnalogInput> pressureSensor;
	#ifdef REAL
		Compressor* compressor;
	#endif

	static constexpr double MAX_PRESSURE = 2.55;

public:
	Pneumatics();

	/**
	 * No default command
	 */
	void InitDefaultCommand();

	/**
	 * Start the compressor going. The compressor automatically starts and stops as it goes above and below maximum pressure.
	 */
	void Start();

	/**
	 * @return Whether or not the system is fully pressurized.
	 */
	bool IsPressurized();

	/**
	 * Puts the pressure on the SmartDashboard.
	 */
	void WritePressure();
};

#endif
