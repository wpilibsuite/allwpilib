#ifndef SetDistanceToBox_H
#define SetDistanceToBox_H

#include <Commands/Command.h>
#include <PIDOutput.h>
#include <PIDSource.h>

namespace frc {
class PIDController;
}

/**
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
class SetDistanceToBox: public frc::Command {
public:
	SetDistanceToBox(double distance);
	void Initialize() override;
	bool IsFinished() override;
	void End() override;
private:
	frc::PIDController* pid;
};

class SetDistanceToBoxPIDSource: public frc::PIDSource {
public:
	virtual ~SetDistanceToBoxPIDSource() = default;
	double PIDGet() override;
};

class SetDistanceToBoxPIDOutput: public frc::PIDOutput {
public:
	virtual ~SetDistanceToBoxPIDOutput() = default;
	void PIDWrite(double d) override;
};

#endif  // SetDistanceToBox_H
