#ifndef DriveForward_H
#define DriveForward_H

#include <Commands/Command.h>

/**
 * This command drives the robot over a given distance with simple proportional
 * control This command will drive a given distance limiting to a maximum speed.
 */
class DriveForward: public frc::Command {
public:
	DriveForward();
	DriveForward(double dist);
	DriveForward(double dist, double maxSpeed);
	void Initialize() override;
	void Execute() override;
	bool IsFinished() override;
	void End() override;

private:
	double driveForwardSpeed;
	double distance;
	double error = 0;
	static constexpr double kTolerance = 0.1;
	static constexpr double kP = -1.0 / 5.0;

	void init(double dist, double maxSpeed);
};

#endif  // DriveForward_H
