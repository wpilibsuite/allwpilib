#ifndef Pneumatics_H
#define Pneumatics_H

#include <AnalogInput.h>
#include <Commands/Subsystem.h>
#include <Compressor.h>

/**
 * The Pneumatics subsystem contains the compressor and a pressure sensor.
 *
 * NOTE: The simulator currently doesn't support the compressor or pressure sensors.
 */
class Pneumatics: public frc::Subsystem {
public:
	Pneumatics();

	/**
	 * No default command
	 */
	void InitDefaultCommand() override;

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

private:
	frc::AnalogInput pressureSensor { 3 };

#ifndef SIMULATION
	frc::Compressor compressor { 1 };  // TODO: (1, 14, 1, 8);
#endif

	static constexpr double kMaxPressure = 2.55;
};

#endif  // Pneumatics_H
