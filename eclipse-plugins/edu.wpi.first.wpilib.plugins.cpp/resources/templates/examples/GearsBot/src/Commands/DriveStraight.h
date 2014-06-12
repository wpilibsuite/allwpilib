#ifndef DriveStraight_H
#define DriveStraight_H

#include "WPILib.h"
#include "Commands/Command.h"

/**
 * Drive the given distance straight (negative values go backwards).
 * Uses a local PID controller to run a simple PID loop that is only
 * enabled while this command is running. The input is the averaged
 * values of the left and right encoders.
 */
class DriveStraight: public Command {
public:
	DriveStraight(double distance);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
private:
	PIDController* pid;
};

class DriveStraightPIDSource: public PIDSource {
public:
	virtual ~DriveStraightPIDSource();
	double PIDGet();
};

class DriveStraightPIDOutput: public PIDOutput {
public:
	virtual ~DriveStraightPIDOutput();
	void PIDWrite(float d);
};

#endif
