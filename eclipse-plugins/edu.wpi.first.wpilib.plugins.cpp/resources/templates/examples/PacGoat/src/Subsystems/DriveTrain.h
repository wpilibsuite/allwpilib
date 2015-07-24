#ifndef DriveTrain_H
#define DriveTrain_H

#include "Commands/Subsystem.h"
#include "WPILib.h"

/**
 * The DriveTrain subsystem controls the robot's chassis and reads in
 * information about it's speed and position.
 */
class DriveTrain: public Subsystem
{
private:
	// Subsystem devices
	std::shared_ptr<SpeedController> frontLeftCIM, frontRightCIM;
	std::shared_ptr<SpeedController> backLeftCIM, backRightCIM;
	RobotDrive drive;
	std::shared_ptr<Encoder> rightEncoder, leftEncoder;
	std::shared_ptr<Gyro> gyro;

public:
	DriveTrain();

	/**
	 * When other commands aren't using the drivetrain, allow tank drive with
	 * the joystick.
	 */
	void InitDefaultCommand();

	/**
	 * @param joy PS3 style joystick to use as the input for tank drive.
	 */
	void TankDrive(Joystick* joy);

	/**
	 * @param leftAxis Left sides value
	 * @param rightAxis Right sides value
	 */
	void TankDrive(double leftAxis, double rightAxis);

	/**
	 * Stop the drivetrain from moving.
	 */
	void Stop();

	/**
	 * @return The encoder getting the distance and speed of left side of the drivetrain.
	 */
  std::shared_ptr<Encoder> GetLeftEncoder();

	/**
	 * @return The encoder getting the distance and speed of right side of the drivetrain.
	 */
  std::shared_ptr<Encoder> GetRightEncoder();

	/**
	 * @return The current angle of the drivetrain.
	 */
	double GetAngle();
};

#endif
