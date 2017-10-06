#ifndef OpenClaw_H
#define OpenClaw_H

#include <Commands/Command.h>

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
class OpenClaw: public frc::Command {
public:
	OpenClaw();
	void Initialize() override;
	bool IsFinished() override;
	void End() override;
};

#endif  // OpenClaw_H
