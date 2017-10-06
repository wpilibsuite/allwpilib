#ifndef OpenClaw_H
#define OpenClaw_H

#include <Commands/Command.h>

/**
 * Opens the claw
 */
class OpenClaw: public frc::Command {
public:
	OpenClaw();
	void Initialize() override;
	bool IsFinished() override;
};

#endif  // OpenClaw_H
