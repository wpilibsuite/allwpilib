#ifndef SetDistanceToBox_H
#define SetDistanceToBox_H

#include "WPILib.h"
#include "Commands/Command.h"

/**
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
class SetDistanceToBox: public Command {
public:
	SetDistanceToBox(double distance);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
private:
	PIDController* pid;
};

class SetDistanceToBoxPIDSource: public PIDSource {
public:
	virtual ~SetDistanceToBoxPIDSource();
	double PIDGet();
};

class SetDistanceToBoxPIDOutput: public PIDOutput {
public:
	virtual ~SetDistanceToBoxPIDOutput();
	void PIDWrite(float d);
};

#endif
