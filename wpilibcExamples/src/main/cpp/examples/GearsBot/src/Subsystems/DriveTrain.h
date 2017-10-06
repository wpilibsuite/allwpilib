#ifndef DriveTrain_H
#define DriveTrain_H

#include <AnalogGyro.h>
#include <AnalogInput.h>
#include <Commands/Subsystem.h>
#include <Encoder.h>
#include <RobotDrive.h>
#include <Talon.h>

namespace frc {
class Joystick;
}

/**
 * The DriveTrain subsystem incorporates the sensors and actuators attached to
 * the robots chassis. These include four drive motors, a left and right encoder
 * and a gyro.
 */
class DriveTrain: public frc::Subsystem {
public:
	DriveTrain();

	/**
	 * When no other command is running let the operator drive around
	 * using the PS3 joystick.
	 */
	void InitDefaultCommand() override;

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
	void Log();

	/**
	 * Tank style driving for the DriveTrain.
	 * @param left Speed in range [-1,1]
	 * @param right Speed in range [-1,1]
	 */
	void Drive(double left, double right);

	/**
	 * @param joy The ps3 style joystick to use to drive tank style.
	 */
	void Drive(frc::Joystick* joy);

	/**
	 * @return The robots heading in degrees.
	 */
	double GetHeading();

	/**
	 * Reset the robots sensors to the zero states.
	 */
	void Reset();

	/**
	 * @return The distance driven (average of left and right encoders).
	 */
	double GetDistance();

	/**
	 * @return The distance to the obstacle detected by the rangefinder.
	 */
	double GetDistanceToObstacle();

private:
	frc::Talon frontLeft { 1 };
	frc::Talon rearLeft { 2 };
	frc::Talon frontRight { 3 };
	frc::Talon rearRight { 4 };
	frc::RobotDrive drive { frontLeft, rearLeft, frontRight, rearRight };
	frc::Encoder leftEncoder { 1, 2 };
	frc::Encoder rightEncoder { 3, 4 };
	frc::AnalogInput rangefinder { 6 };
	frc::AnalogGyro gyro { 1 };
};

#endif  // DriveTrain_H
