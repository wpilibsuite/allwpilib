#ifndef DriveStraight_H
#define DriveStraight_H

#include <Commands/Command.h>
#include <PIDOutput.h>
#include <PIDSource.h>

namespace frc {
class PIDController;
}

/**
 * Drive the given distance straight (negative values go backwards).
 * Uses a local PID controller to run a simple PID loop that is only
 * enabled while this command is running. The input is the averaged
 * values of the left and right encoders.
 */
class DriveStraight: public frc::Command {
public:
	DriveStraight(double distance);
	void Initialize() override;
	bool IsFinished() override;
	void End() override;
private:
	frc::PIDController* pid;
};

class DriveStraightPIDSource: public PIDSource {
public:
	virtual ~DriveStraightPIDSource() = default;
	double PIDGet() override;
};

class DriveStraightPIDOutput: public PIDOutput {
public:
	virtual ~DriveStraightPIDOutput() = default;
	void PIDWrite(double d) override;
};

#endif  // DriveStraight_H
